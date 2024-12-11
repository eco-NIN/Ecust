package org.example.erp.controller;

import jakarta.annotation.Resource;
import org.example.erp.dto.InputDTO;
import org.example.erp.dto.ResultDTO;
import org.example.erp.dto.ResultQueueDTO;
import org.example.erp.dto.TreeNodeMapDTO;
import org.example.erp.entity.TreeNode;
import org.example.erp.service.FormulaService;
import org.example.erp.service.StockConsumptionService;
import org.example.erp.service.TreeBuildingService;
import org.example.erp.service.TreeSearchingService;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;

/**
 * @author hutianlin
 * 2024/10/10 15:46
 */
@RestController
@RequestMapping("/getERPAnswer")
@CrossOrigin(origins = "*", allowedHeaders = "*")
public class ERPController {

    @Resource
    private TreeBuildingService treeBuildingService;

    @Resource
    private TreeSearchingService treeSearchingService;

    @Resource
    private StockConsumptionService stockConsumptionService;

    @Resource
    private FormulaService formulaService;

    @PostMapping("/getERPResult")
    public List<ResultDTO> getERPResult(@RequestBody List<InputDTO> inputListDTO) {
        TreeNodeMapDTO treeNodeMapDTO = new TreeNodeMapDTO();
        List<TreeNode> rootList = new ArrayList<>();
        ResultQueueDTO resultQueueDTO = new ResultQueueDTO();
        List<ResultDTO> resultDTOList = new ArrayList<>();

        for (InputDTO inputDTO : inputListDTO) {
            TreeNode root = treeBuildingService.setupRoot(inputDTO.getName(), inputDTO.getDemand(), inputDTO.getCompletionDate());
            treeBuildingService.buildTree(root);
            treeSearchingService.getTreeNodeQueueMap(root, treeNodeMapDTO);
            rootList.add(root);
        }

        //消耗中间件库存
        stockConsumptionService.consumeMiddlewareStock(treeNodeMapDTO);
        treeNodeMapDTO.getNodeQueueMap().clear();

        //重新获取节点队列
        for (TreeNode root : rootList) {
            treeSearchingService.getTreeNodeQueueMap(root, treeNodeMapDTO);
        }

        //消耗采购件库存
        stockConsumptionService.consumeStock(treeNodeMapDTO);

        for (TreeNode root : rootList) {
            treeSearchingService.getResult(root, resultQueueDTO);
        }

        while (!resultQueueDTO.getResults().isEmpty()) {
            resultDTOList.add(resultQueueDTO.getResults().poll());
        }

        return resultDTOList;
    }

    @PostMapping("/getFormulaResult")
    public List<String> getFormulaResult(@RequestBody List<String> variableIds) {
        List<String> result = new ArrayList<>();

        for (String variableId : variableIds) {
            result.add(formulaService.generateFormula(variableId));
        }

        return result;
    }
}
