package org.example.erp.service;

import jakarta.annotation.Resource;
import org.example.erp.dto.ResultQueueDTO;
import org.example.erp.dto.TreeNodeMapDTO;
import org.example.erp.entity.TreeNode;
import org.example.erp.dto.InputDTO;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest
class TreeBuildingServiceTest {

    @Resource
    private TreeBuildingService treeBuildingService;

    @Resource
    private TreeSearchingService treeSearchingService;

    @Resource
    private StockConsumptionService stockConsumptionService;

    @Test
    void testBuildTree() {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");

        // Arrange
        InputDTO inputDTO = new InputDTO();
        inputDTO.setName("眼镜");
        inputDTO.setDemand(100);
        inputDTO.setCompletionDate(LocalDate.parse("2020-05-30", formatter));

        TreeNodeMapDTO treeNodeMapDTO = new TreeNodeMapDTO();
        List<TreeNode> rootList = new ArrayList<>();
        ResultQueueDTO resultQueueDTO = new ResultQueueDTO();

        TreeNode root = treeBuildingService.setupRoot(inputDTO.getName(), inputDTO.getDemand(), inputDTO.getCompletionDate());
        treeBuildingService.buildTree(root);
        treeSearchingService.getTreeNodeQueueMap(root, treeNodeMapDTO);
        rootList.add(root);

        /*inputDTO.setName("镜框");
        inputDTO.setDemand(10);
        inputDTO.setCompletionDate(LocalDate.parse("2020-05-01", formatter));

        root = treeBuildingService.setupRoot(inputDTO.getName(), inputDTO.getDemand(), inputDTO.getCompletionDate());
        treeBuildingService.buildTree(root);
        treeSearchingService.getTreeNodeQueueMap(root, treeNodeMapDTO);
        rootList.add(root);*/


        stockConsumptionService.consumeMiddlewareStock(treeNodeMapDTO);

        dfs(root);

    }

    private void dfs(TreeNode node) {
        System.out.println(node.toString());

        for (TreeNode child : node.getChildren()) {
            dfs(child);
        }
    }
}