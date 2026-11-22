#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/glm.h>

namespace danikk_framework
{
	/** @brief Shared geometry and child links for one cell in a 3D octree. */
	template<typename node_id_t = uint32_t>
	struct OctreeNode
	{
		/** @brief Sentinel used for missing child and root IDs. */
		static constexpr node_id_t invalid_node_id = static_cast<node_id_t>(-1);

		/** @brief Center of this cubic cell. */
		glm::dvec3 center = glm::dvec3(0.0);
		/** @brief Half the side length of this cell. */
		double half_extent = 0.0;
		/** @brief IDs of the eight octants, or invalid_node_id for an absent child. */
		node_id_t children[8] = {
			invalid_node_id, invalid_node_id, invalid_node_id, invalid_node_id,
			invalid_node_id, invalid_node_id, invalid_node_id, invalid_node_id};
		/** @brief Whether any child octant has been allocated. */
		bool hasChildren() const
		{
			for(node_id_t child_id : children)
				if(child_id != invalid_node_id) return true;
			return false;
		}
	};

	/** @brief Contiguous, index-addressed storage and topology operations for a 3D octree. */
	template<typename node_t, typename node_id_t = uint32_t>
	class Octree
	{
		static_assert(std::is_base_of_v<OctreeNode<node_id_t>, node_t>,
			"Octree node types must derive from OctreeNode<node_id_t>");

	public:
		using Node = node_t;
		using NodeId = node_id_t;
		static constexpr NodeId invalid_node_id = OctreeNode<NodeId>::invalid_node_id;

		/** @brief All node payloads and topology, stored contiguously and referenced by index. */
		DynamicArray<Node> nodes;
		/** @brief Root node ID, or invalid_node_id when the tree has no root. */
		NodeId root_id = invalid_node_id;

		/** @brief Appends a leaf node with the supplied cubic bounds. */
		NodeId appendNode(const glm::dvec3& center, double half_extent)
		{
			if(nodes.size() >= static_cast<size_t>(invalid_node_id)) return invalid_node_id;
			const NodeId node_id = static_cast<NodeId>(nodes.size());
			Node& node = nodes.pushCtor();
			node.center = center;
			node.half_extent = half_extent;
			return node_id;
		}

		/** @brief Creates a root node if the tree is currently empty. */
		NodeId createRoot(const glm::dvec3& center, double half_extent)
		{
			if(root_id != invalid_node_id) return root_id;
			root_id = appendNode(center, half_extent);
			return root_id;
		}

		/** @brief Creates all eight half-size children of a leaf node. */
		bool splitNode(NodeId parent_id)
		{
			if(parent_id >= nodes.size()) return false;
			if(nodes[parent_id].hasChildren()) return true;
			if(nodes.size() > static_cast<size_t>(invalid_node_id) - 8) return false;

			const size_t original_node_count = nodes.size();
			const glm::dvec3 parent_center = nodes[parent_id].center;
			const double child_half_extent = nodes[parent_id].half_extent * 0.5;
			NodeId child_ids[8];
			for(uint8_t index = 0; index < 8; ++index)
			{
				const glm::dvec3 offset(
					(index & 1) ? child_half_extent : -child_half_extent,
					(index & 2) ? child_half_extent : -child_half_extent,
					(index & 4) ? child_half_extent : -child_half_extent);
				child_ids[index] = appendNode(parent_center + offset, child_half_extent);
				if(child_ids[index] == invalid_node_id)
				{
					nodes.resize(original_node_count);
					return false;
				}
			}
			for(uint8_t index = 0; index < 8; ++index)
				nodes[parent_id].children[index] = child_ids[index];
			return true;
		}

		/** @brief Wraps the current root in a larger sparse parent, preserving its octant. */
		NodeId wrapRoot(const glm::dvec3& center, double half_extent, uint8_t old_root_child_index)
		{
			if(old_root_child_index >= 8 || root_id == invalid_node_id) return invalid_node_id;
			const NodeId old_root_id = root_id;
			const NodeId new_root_id = appendNode(center, half_extent);
			if(new_root_id == invalid_node_id) return invalid_node_id;
			nodes[new_root_id].children[old_root_child_index] = old_root_id;
			root_id = new_root_id;
			return new_root_id;
		}

		/** @brief Clears node storage and resets the root while retaining array capacity. */
		void clear()
		{
			nodes.clear();
			root_id = invalid_node_id;
		}
	};
}
