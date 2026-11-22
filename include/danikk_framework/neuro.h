#pragma once
#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/array.h>
#include <danikk_framework/random.h>
#include <danikk_framework/log.h>
#include <danikk_framework/sub_array.h>
#include <danikk_framework/math.h>
#include <danikk_framework/thread.h>
#include <danikk_framework/hex.h>
#include "memory_buffer.h"

#define BINARY_WRITE_FUNC template<class streamT> void binaryWrite(streamT& out)
#define BINARY_READ_FUNC template<class streamT> void binaryRead(streamT& in)
#define BINARY_READ1_FUNC template<class streamT> void binaryRead1(streamT& in)
#define BINARY_READ2_FUNC template<class streamT> void binaryRead2(streamT& in)

#define MULTI_TASK_ACTIVATE_TASK_COUNT 8
#define MULTI_TASK_MUTATE_TASK_COUNT 16

namespace danikk_framework
{
	class GenericNeuroNetwork
	{
	protected:
	};

	class NeuroNetwork : public GenericNeuroNetwork
	{
	public:
		struct neuron
		{
			float k;
			float v;

			neuron() = default;

			neuron(float default_neuro_k) : k(default_neuro_k){}
		};
	private:
		static ThreadPool& getTaskThreadPool()
		{
			static ThreadPool pool;
			static const bool initialized = (pool.initialize(MULTI_TASK_MUTATE_TASK_COUNT), true);
			(void)initialized;
			return pool;
		}

		struct layer
		{
			DynamicArray<neuron> neurons;
			DynamicArray<float> weights;

			layer() = default;

			layer(size_t neurons_count, size_t weights_count, float default_weight, float default_neuro_k)
			{
				neurons.resize(neurons_count, default_neuro_k);
				weights.resize(weights_count, default_weight);
			}

			/*layer& operator=(const layer& other)
			{
				this->neurons = other.neurons;
				this->weights = other.weights;
				return *this;
			}*/

			BINARY_WRITE_FUNC
			{
				out << (uint16)neurons.size();
				for(neuron& neuron : neurons)
				{
					out << neuron.k;
				}
				for(float& weight : weights)
				{
					out << weight;
				}
			}

			BINARY_READ1_FUNC
			{
				uint16 neurons_size;
				in >> neurons_size;
				neurons.resize(neurons_size);
			}

			BINARY_READ2_FUNC
			{
				for(neuron& neuron : neurons)
				{
					in >> neuron.k;
				}
				for(float& weight : weights)
				{
					in >> weight;
				}
			}
		};

		DynamicArray<layer> layers;
	public:
		NeuroNetwork() = default;

		NeuroNetwork(NeuroNetwork&& other)
		{
			this->layers = move(other.layers);
		}

		NeuroNetwork& operator=(NeuroNetwork& source)
		{
			layers = source.layers;

			return *this;
		}

		NeuroNetwork& operator=(NeuroNetwork&& other) = delete;

		template<size_t layers_count> void layersInit(const Array<uint32,layers_count>& layer_sizes, float default_weights = 1.0f, float default_neuro_k = 1.0f)
		{
			assert(layers.size() == 0);

			layer* prev = NULL;
			for(uint32 layer_size : layer_sizes)
			{
				prev = &layers.pushCtor(layer_size, prev ? layer_size * prev->neurons.size() : 0, default_weights, default_neuro_k);
			}
		}

		template<size_t first_layer_size> void setInput(const Array<float, first_layer_size>& input)
		{
			DynamicArray<neuron>* neurons = &layers[0].neurons;
			assert(neurons->size() == input.size());
			memcpy((void*)neurons->data(), (const void*)input.data(), input.size() * sizeof(float));
		}

		DynamicArray<neuron>& getInputNeurons()
		{
			return layers[0].neurons;
		}

		DynamicArray<neuron>& getOutNeurons()
		{
			return layers.last().neurons;
		}

		struct activatorTaskSlot
		{
			Task task;
			SubArray<float> weights;
			SubArray<neuron> neurons;
			SubArray<neuron> prev_neurons;
		};

		static void multiTaskSubActivate(void* task_ptr)
		{
			activatorTaskSlot& task = *(activatorTaskSlot*)task_ptr;

			index_t weight_index = 0;
			for(neuron& cur_neuron : task.neurons)
			{
				cur_neuron.v = 0;
				for(neuron& prev_neuron : task.prev_neurons)
				{
					cur_neuron.v += prev_neuron.v * task.weights[weight_index++] / (float)task.prev_neurons.size();
				}
				cur_neuron.v *= cur_neuron.k;
			}
		}

		void multiTaskActivate()
		{
			activatorTaskSlot tasks[MULTI_TASK_ACTIVATE_TASK_COUNT];

			layer* prev = &layers[0];
			for(index_t layer_index = 1; layer_index < layers.size(); layer_index++)
			{
				layer* cur = &layers[layer_index];
				index_t weight_index = 0;
				if(cur->neurons.size() < 1024)//при небольшом размере блока происходит синхронное вычисление, при большом асинхронное
				{
					sync_calc:
					for(neuron& cur_neuron : cur->neurons)
					{
						cur_neuron.v = 0;
						for(neuron& prev_neuron : prev->neurons)
						{
							cur_neuron.v += prev_neuron.v * cur->weights[weight_index++] / (float)prev->neurons.size();
						}
						cur_neuron.v *= cur_neuron.k;
					}
				}
				else if(isPowOf2(cur->neurons.size()))
				{
					size_t target_block_size = cur->neurons.size() / MULTI_TASK_ACTIVATE_TASK_COUNT;
					size_t target_weights_block_size = target_block_size * prev->neurons.size();
					index_t neuron_offset = 0;
					index_t weight_offset = 0;
					for(activatorTaskSlot& task : tasks)
					{
						task.weights = SubArray<float>(cur->weights.data() + weight_offset, target_weights_block_size);
						task.neurons = SubArray<neuron>(cur->neurons.data() + neuron_offset, target_block_size);
						task.prev_neurons = SubArray<neuron>(prev->neurons.data(), prev->neurons.size());
						task.task = getTaskThreadPool().submit(Task(multiTaskSubActivate, &task));
						neuron_offset += target_block_size;
						weight_offset += target_block_size * task.prev_neurons.size();
					}
					for(activatorTaskSlot& task : tasks)
					{
						task.task.waitFor();
					}
				}
				else
				{
					logWarning("(!isPowOf2(target_block_size)) && cur->neurons.size() > 1024");
					goto sync_calc;
				}

				prev = cur;
			}
		}

		void activate()
		{
			layer* prev = &layers[0];
			for(index_t layer_index = 1; layer_index < layers.size(); layer_index++)
			{
				layer* cur = &layers[layer_index];
				index_t weight_index = 0;
				for(neuron& cur_neuron : cur->neurons)
				{
					cur_neuron.v = 0;
					for(neuron& prev_neuron : prev->neurons)
					{
						cur_neuron.v += prev_neuron.v * cur->weights[weight_index++] / (float)prev->neurons.size();
					}
					cur_neuron.v *= cur_neuron.k;
				}
				prev = cur;
			}
		}

		void logValue()
		{
			if(!startLogDebug()) return;
			for(index_t layer_index = 0; layer_index < layers.size(); layer_index++)
			{
				layer* cur = &layers[layer_index];
				formatAppend(log_buffer, "\nlayer:% ", layer_index);
				int counter = 0;
				for(neuron& cur_neuron : cur->neurons)
				{
					counter++;
					if(counter > 32)
					{
						break;
					}
					log_buffer << cur_neuron.v << ' ';
				}
			}
			endLog();
		}

		void logLayersAvgNeuro()
		{
			if(!startLogDebug()) return;
			for(index_t layer_index = 0; layer_index < layers.size(); layer_index++)
			{
				layer* cur = &layers[layer_index];
				float avg_k = 0;
				float avg_w = 0;
				for(neuron& cur_neuron : cur->neurons)
				{
					avg_k += cur_neuron.k;
				}
				for(float& cur_w : cur->weights)
				{
					avg_w += cur_w;
				}
				avg_k /= cur->neurons.size();
				avg_w /= cur->weights.size();
				formatAppend(log_buffer, "\nlayer:% avg_k:% avg_w:%", layer_index, avg_k, avg_w);
			}
			endLog();
		}

		void logOutputValue()
		{
			layer& l = layers.last();

			if(!startLogDebug()) return;
			for(neuron& n : l.neurons)
			{
				log_buffer << n.v << ' ';
			}
			endLog();
		}

		void logAvgOutputValue()
		{
			layer& l = layers.last();

			float avg = 0;
			for(neuron& n : l.neurons)
			{
				avg += n.v;
			}
			avg /= l.neurons.size();
			logInfo(avg);
		}

		void logAvgNeuro()
		{
			if(!startLogDebug()) return;

			float res_avg_k = 0;
			float res_avg_w = 0;

			for(index_t layer_index = 0; layer_index < layers.size(); layer_index++)
			{
				layer* cur = &layers[layer_index];
				float avg_k = 0;
				float avg_w = 0;
				for(neuron& cur_neuron : cur->neurons)
				{
					avg_k += cur_neuron.k;
				}
				for(float& cur_w : cur->weights)
				{
					avg_w += cur_w;
				}
				res_avg_k += avg_k / cur->neurons.size();
				res_avg_w += avg_w / cur->weights.size();
			}
			formatLogInfo("avg_k:% avg_w:%", res_avg_k, res_avg_w);
			endLog();
		}

		template<size_t last_layer_size> void getOutput(Array<float, last_layer_size>& output)
		{
			DynamicArray<neuron>& neurons = layers.last().neurons;
			assert(neurons.size() == output.size());
			for(index_t i = 0; i < output.size(); i++)
			{
				output[i] = neurons[i].v;
			}
		}


		struct mutateTaskSlot
		{
			Task task;
			float step;
			bool32 is_weight_mutate = 0;//структура таска универсальная, как для нейронов, так и для меж нейронных весов
			DynamicArray<float>* weights;//тут должен был быть union, но не получилось (
			DynamicArray<neuron>* neurons;
			random_booler booler;
		};

		static void multiTaskSubMutate(void* task_ptr)
		{
			mutateTaskSlot& task = *(mutateTaskSlot*)task_ptr;
			float& step = task.step;
			if(task.is_weight_mutate)
			{
				for(float& weight : *task.weights)
				{
					weight += task.booler.next() ? step : -step;
				}
			}
			else
			{
				for(neuron& neuron : *task.neurons)
				{
					neuron.k += task.booler.next() ? step : -step;
				}
			}
		}

		void multiTaskMutate(float step = 0.001f)
		{
			struct task_container
			{
				mutateTaskSlot tasks[MULTI_TASK_MUTATE_TASK_COUNT];

				mutateTaskSlot* get()
				{
					for(mutateTaskSlot& task : tasks)
					{
						if(task.task.isEnd())
						{
							return &task;
						}
					}
					if(!tasks[0].task.isEnd())
					{
						tasks[0].task.waitFor();
					}
					return &tasks[0];
				}

				void waitFor()
				{
					for(mutateTaskSlot& task : tasks)
					{
						task.task.waitFor();
					}
				}
			};

			task_container task_container;
			mutateTaskSlot* free_task;

			for(layer& layer : layers)
			{
				free_task = task_container.get();
				free_task->is_weight_mutate = false;
				free_task->step = step;
				free_task->neurons = &layer.neurons;
				free_task->task = getTaskThreadPool().submit(Task(multiTaskSubMutate, free_task));

				free_task = task_container.get();
				free_task->is_weight_mutate = true;
				free_task->step = step;
				free_task->weights = &layer.weights;
				free_task->task = getTaskThreadPool().submit(Task(multiTaskSubMutate, free_task));
			}
			task_container.waitFor();
		}

		static float getMutateFactor(Random& rand, float step)
		{
			uint8 random = rand.number<uint8>();
			float scale_factor = (random >> 1) + 1 % 10;
			float sign_factor = random & 1 ? 1 : -1;
			return step * scale_factor * sign_factor;
		}

		void mutate(float step = 0.001f)
		{
			Random random;
			random.init();
			for(layer& layer : layers)
			{
				for(neuron& neuron : layer.neurons)
				{
					neuron.k += getMutateFactor(random, step);
				}
				for(float& weight : layer.weights)
				{
					weight += getMutateFactor(random, step);
				}
			}
		}

		BINARY_WRITE_FUNC
		{
			out << (uint8)layers.size();
			for(layer& layer : layers)
			{
				layer.binaryWrite(out);
			}
		}

		BINARY_READ_FUNC
		{
			{
				uint8 layers_size;
				in >> layers_size;
				layers.resize(layers_size, layer());
			}
			layer* prev = NULL;
			for(index_t i = 0; i < layers.size(); i++)
			{
				layer* cur = &layers[i];
				cur->binaryRead1(in);
				cur->weights.resize(prev ? cur->neurons.size() * prev->neurons.size() : 0);
				cur->binaryRead2(in);
				prev = cur;
			}
		}
	};
}







//Класс нейросети с фиксированным количеством нейронов.
//Все данные хранятся в самом классе без использования кучи.
//НЕ РАБОТАЕТ
/*template<size_t layers_count, size_t neuron_count, size_t neuroscale_count> class FixedNeuroNetwork : public GenericNeuroNetwork
{
private:
	float neuroscale_buffer[neuroscale_count]{0};

	struct neuron
	{
		float k = 1;
		float v;
		uint32 neuroscale_index;

		BINARY_WRITE_FUNC
		{
			out >> k;
		}

		BINARY_READ_FUNC
		{
			in << k;
		}
	};

	typedef ArrayOfArray<layers_count, neuron_count, neuron> data_container_t;

	data_container_t data;

	void initNeuroscale()
	{
		SubArray<neuron> prev = data[0];

		size_t current_neuroscale_index = 0;
		for(size_t index = 1; index < layers_count; index++)
		{
			SubArray<neuron> arr = data[index];

			for(neuron& neuron : arr)
			{
				neuron.neuroscale_index = current_neuroscale_index;
				current_neuroscale_index += prev.size();
			}

			prev = arr;
		}
	}
public:
	FixedNeuroNetwork() = default;

	FixedNeuroNetwork(InitList<size_t> layers_size) : data(layers_size)
	{
		assert(layers_size.size() == layers_count);
		initNeuroscale();
	}

	FixedNeuroNetwork(Array<size_t, layers_count> layers_size) : data(layers_size)
	{
		assert(layers_size.size() == layers_count);
		initNeuroscale();
	}


	uint64 metadataHash()
	{
		return data.metadataHash() + neuroscale_count;
	}

	void defaultInit()
	{
		data.setValue(1);
		for(float& f : neuroscale_buffer)
		{
			f = 1;
		}
	}

	BINARY_WRITE_FUNC
	{
		for(float& f : neuroscale_buffer)
		{
			out << f;
		}
		for(neuron& neuro : data)
		{
			neuro.binaryWrite(out);
		}
	}

	BINARY_READ_FUNC
	{
		for(float& f : neuroscale_buffer)
		{
			in << f;
		}
		for(neuron& neuro : data)
		{
			neuro.binaryRead(in);
		}
	}

	template<size_t first_layer_size> void setInput(Array<float, first_layer_size> input)
	{
		SubArray<neuron> arr = data[0];
		assert(arr.size() == first_layer_size);
		for(size_t index = 0; index < first_layer_size; index++)
		{
			arr[index].v = input[index];
		}
	}

	void activate()
	{
		SubArray<neuron> prev = data[0];
		for(size_t index = 1; index < layers_count; index++)
		{
			SubArray<neuron> arr = data[index];

			for(size_t sub_index = 0; sub_index < arr.size(); sub_index++)
			{
				neuron& neuro = arr[sub_index];
				neuro.v = 0;

				for(size_t prev_neuron_index = 0; prev_neuron_index < prev.size(); prev_neuron_index++)
				{
					float prevv = prev[prev_neuron_index].v;
					float scale = neuroscale_buffer[neuro.neuroscale_index + prev_neuron_index];
					neuro.v += prevv * scale;
				}
				neuro.v = neuro.v * neuro.k;
			}

			prev = arr;
		}
	}

	void mutate()
	{
		for(neuron& neuro : data)
		{
			floatMutate(neuro.k);
		}
		for(float& scale : neuroscale_buffer)
		{
			floatMutate(scale);
		}
	}

	template<size_t last_layer_size> void getOutput(Array<float, last_layer_size>& out)
	{
		SubArray<neuron> arr = data[layers_count-1];
		assert(arr.size() == last_layer_size);
		out.resize(last_layer_size);
		for(size_t index = 0; index < last_layer_size; index++)
		{
			out[index] = arr[index].v;
		}
	}
};*/
