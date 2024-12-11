package org.example.erp.service;

import org.example.erp.dto.TreeNodeMapDTO;

/**
 * @author hutianlin
 * 2024/10/11 11:26
 * 库存扣除服务接口
 */
public interface StockConsumptionService {

    void consumeStock(TreeNodeMapDTO treeNodeMapDTO);

    void consumeMiddlewareStock(TreeNodeMapDTO treeNodeMapDTO);
}
