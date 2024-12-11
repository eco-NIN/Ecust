package org.example.erp.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDate;
import java.util.LinkedList;
import java.util.List;

/**
 * @author hutianlin
 * 2024/10/11 00:37
 * 树节点
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class TreeNode {

    /**
     * 物料号
     */
    private String id;

    /**
     * 父节点物料号
     * 如果 id == parentId 则为根节点
     */
    private String parentId;

    /**
     * 物料名
     */
    private String name;

    /**
     * 日程下达日期
     */
    private LocalDate orderDate;

    /**
     * 日程完成日期
     */
    private LocalDate completionDate;

    /**
     * 调配方式
     */
    private String method;

    /**
     * 需求量
     */
    private Integer demand = 0;

    /**
     * 构成数
     */
    private Integer number;

    /**
     * 子节点
     */
    List<TreeNode> children = new LinkedList<>();
}
