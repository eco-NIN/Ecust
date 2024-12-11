package org.example.erp.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author hutianlin
 * 2024/10/10 15:49
 * 库存表
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class Stock {

    /**
     * 物料号
     */
    private String id;

    /**
     * 工序库存
     */
    private Integer processInventory;

    /**
     * 资材库存
     */
    private Integer materialInventory;

    /**
     * 中间件库存
     */
    private Integer middlewareInventory;

}
