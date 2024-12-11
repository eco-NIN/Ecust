package org.example.erp.service;

import org.example.erp.dto.ResultQueueDTO;
import org.example.erp.dto.TreeNodeMapDTO;
import org.example.erp.entity.TreeNode;

/**
 * @author hutianlin
 * 2024/10/11 10:39
 * 树节点搜索服务接口
 */
public interface TreeSearchingService {

    /**
     * 获取树节点优先队列map
     */
    void getTreeNodeQueueMap(TreeNode treeNode, TreeNodeMapDTO treeNodeMapDTO);

    /**
     * 获取结果
     */
    void getResult(TreeNode treeNode, ResultQueueDTO resultQueueDTO);
}
