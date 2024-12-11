package org.example.erp.service;

import org.example.erp.entity.TreeNode;

import java.time.LocalDate;

/**
 * @author hutianlin
 * 2024/10/11 00:50
 * 树的构建服务接口
 */
public interface TreeBuildingService {

    /**
     * 构建树
     */
    void buildTree(TreeNode node);

    /**
     * 初始化根节点
     */
    TreeNode setupRoot(String name, Integer demand, LocalDate completionDate);

    /**
     * 中间件库存消耗后重构子树
     */
    void reBuildTree(TreeNode node);
}
