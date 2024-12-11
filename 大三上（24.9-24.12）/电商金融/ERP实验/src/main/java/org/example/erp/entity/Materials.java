package org.example.erp.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author hutianlin
 * 2024/10/10 15:49
 * 物料表
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class Materials {

    /**
     * 物料号
     */
    private String id;

    /**
     * 物料名
     */
    private String name;

    /**
     * 调配方式：0采购，1生产
     */
    private String method;

    /**
     * 损耗率
     */
    private Double lossRate;

    /**
     * 作业提前期
     */
    private Integer operationLeadTime;

    /**
     * 物料单位
     */
    private String unit;
}
