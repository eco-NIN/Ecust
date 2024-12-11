package org.example.erp.dto;

import lombok.Data;
import org.example.erp.entity.TreeNode;

import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

/**
 * @author hutianlin
 * 2024/10/11 11:08
 */
@Data
public class TreeNodeMapDTO {
    private Map<String, Queue<TreeNode>> nodeQueueMap = new HashMap<>();
}
