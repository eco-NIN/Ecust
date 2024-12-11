package org.example.erp.utils;

import org.example.erp.entity.TreeNode;

import java.util.Comparator;

/**
 * @author hutianlin
 * 2024/10/11 10:26
 */
public class TreeNodeComparator implements Comparator<TreeNode> {
    public int compare(TreeNode o1, TreeNode o2) {
        return o1.getOrderDate().compareTo(o2.getOrderDate());
    }
}
