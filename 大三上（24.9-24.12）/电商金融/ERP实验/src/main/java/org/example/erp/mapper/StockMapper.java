package org.example.erp.mapper;

import org.apache.ibatis.annotations.Mapper;

/**
 * @author hutianlin
 * 2024/10/11 11:53
 */
@Mapper
public interface StockMapper {

    Integer getProcessInventory(String id);

    Integer getMaterialInventory(String id);

    Integer getMiddlewareInventory(String id);
}
