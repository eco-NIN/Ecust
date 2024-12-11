package org.example.erp.service.impl;


import org.example.erp.dto.ResultDTO;
import org.example.erp.dto.ResultQueueDTO;
import org.example.erp.dto.TreeNodeMapDTO;
import org.example.erp.entity.TreeNode;
import org.example.erp.service.TreeSearchingService;
import org.example.erp.utils.TreeNodeComparator;
import org.springframework.stereotype.Service;

import java.util.PriorityQueue;
import java.util.Queue;

/**
 * @author hutianlin
 * 2024/10/11 10:48
 */
@Service
public class TreeSearchingServiceImpl implements TreeSearchingService {

    @Override
    public void getTreeNodeQueueMap(TreeNode treeNode, TreeNodeMapDTO treeNodeMapDTO) {
        dfsTree(treeNode, treeNodeMapDTO);
    }

    /**
     * 获取结果
     */
    @Override
    public void getResult(TreeNode treeNode, ResultQueueDTO resultQueueDTO) {
        dfsTree(treeNode, resultQueueDTO);
    }


    /**
     * dfs搜索树，获取节点队列映射
     */
    private void dfsTree(TreeNode treeNode, TreeNodeMapDTO treeNodeMapDTO) {

        if (treeNode.getDemand() == null || treeNode.getDemand() <= 0) {
            return;
        }

        if (treeNodeMapDTO.getNodeQueueMap().containsKey(treeNode.getId())) {
            treeNodeMapDTO.getNodeQueueMap().get(treeNode.getId()).offer(treeNode);
        }
        else {
            Queue<TreeNode> queue = new PriorityQueue<>(new TreeNodeComparator());
            queue.offer(treeNode);
            treeNodeMapDTO.getNodeQueueMap().put(treeNode.getId(), queue);
        }

        for (TreeNode child : treeNode.getChildren()) {
            dfsTree(child, treeNodeMapDTO);
        }
    }

    /**
     * dfs搜索树，获取结果队列
     */
    private void dfsTree(TreeNode treeNode, ResultQueueDTO resultQueueDTO) {
        if (treeNode.getDemand() != null && treeNode.getDemand() > 0) {
            ResultDTO resultDTO = new ResultDTO();
            String method = treeNode.getMethod();

            resultDTO.setID(treeNode.getId());
            resultDTO.setName(treeNode.getName());
            resultDTO.setDemand(treeNode.getDemand());
            resultDTO.setOrderDate(treeNode.getOrderDate());
            resultDTO.setCompletionDate(treeNode.getCompletionDate());

            if ("0".equals(method)) {
                resultDTO.setMethod("采购");
            }
            else if ("1".equals(method)) {
                resultDTO.setMethod("生产");
            }

            resultQueueDTO.getResults().offer(resultDTO);

            for (TreeNode child : treeNode.getChildren()) {
                dfsTree(child, resultQueueDTO);
            }
        }
    }
}
