package org.example.erp.service.impl;

import jakarta.annotation.Resource;
import org.example.erp.dto.TreeNodeMapDTO;
import org.example.erp.entity.TreeNode;
import org.example.erp.mapper.ConstituteMapper;
import org.example.erp.mapper.StockMapper;
import org.example.erp.service.StockConsumptionService;
import org.example.erp.service.TreeBuildingService;
import org.springframework.stereotype.Service;

/**
 * @author hutianlin
 * 2024/10/11 11:52
 */
@Service
public class StockConsumptionServiceImpl implements StockConsumptionService {

    @Resource
    private TreeBuildingService treeBuildingService;

    @Resource
    private StockMapper stockMapper;

    @Resource
    private ConstituteMapper constituteMapper;

    @Override
    public void consumeStock(TreeNodeMapDTO treeNodeMapDTO) {
        for (String key : treeNodeMapDTO.getNodeQueueMap().keySet()) {

            Integer processInventory = stockMapper.getProcessInventory(key);
            Integer materialInventory = stockMapper.getMaterialInventory(key);
            Integer middlewareInventory = stockMapper.getMiddlewareInventory(key);

            while (!treeNodeMapDTO.getNodeQueueMap().get(key).isEmpty() && (processInventory > 0 || materialInventory > 0)) {

                TreeNode treeNode = treeNodeMapDTO.getNodeQueueMap().get(key).poll();

                //消耗采购件库存
                if (treeNode != null && treeNode.getMethod() != null && "1".equals(treeNode.getMethod())) {
                    continue;
                }

                if (treeNode != null && treeNode.getDemand() != null) {
                    if (processInventory >= treeNode.getDemand()) {
                        processInventory -= treeNode.getDemand();
                        treeNode.setDemand(0);
                    }
                    else if (processInventory + materialInventory >= treeNode.getDemand()) {
                        materialInventory -= (treeNode.getDemand() - processInventory);
                        processInventory = 0;
                        treeNode.setDemand(0);

                    }
                    else {
                        treeNode.setDemand(treeNode.getDemand() - processInventory - materialInventory);
                        processInventory = 0;
                        materialInventory = 0;
                    }
                }
            }
        }
    }

    @Override
    public void consumeMiddlewareStock(TreeNodeMapDTO treeNodeMapDTO) {
        for (String key : treeNodeMapDTO.getNodeQueueMap().keySet()) {

            Integer middlewareInventory = stockMapper.getMiddlewareInventory(key);

            while (!treeNodeMapDTO.getNodeQueueMap().get(key).isEmpty() && middlewareInventory > 0) {

                TreeNode treeNode = treeNodeMapDTO.getNodeQueueMap().get(key).poll();

                //只消耗中间件库存
                if (treeNode != null && treeNode.getMethod() != null &&  "0".equals(treeNode.getMethod())) {
                    continue;
                }

                if (treeNode != null && treeNode.getDemand() != null && treeNode.getNumber() > 0) {
                    if (middlewareInventory >= treeNode.getDemand()) {
                        middlewareInventory -= treeNode.getDemand();
                        treeNode.setDemand(0);
                    }
                    else {
                        treeNode.setDemand(treeNode.getDemand() - middlewareInventory);
                        middlewareInventory = 0;
                    }
                    //重构子树
                    treeBuildingService.reBuildTree(treeNode);
                }
            }
        }
    }
}
