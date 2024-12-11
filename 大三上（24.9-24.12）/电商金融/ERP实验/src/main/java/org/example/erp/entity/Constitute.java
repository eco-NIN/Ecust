package org.example.erp.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author hutianlin
 * 2024/10/10 15:49
 * 调配构成表
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class Constitute {

    /**
     * 调配基准编号
     */
    private String baseId;

    /**
     * 调配区代码
     */
    private String areaId;

    /**
     * 父物料号
     */
    private String parentId;

    /**
     * 子物料号
     */
    private String childId;

    /**
     * 构成数
     */
    private Integer number;

    /**
     * 配料提前期
     */
    private Integer ingredientsLeadTime;

    /**
     * 供应商提前期
     */
    private Integer supplyLeadTime;

    /**
     * 父物料名称
     */
    private String parentName;

    /**
     * 子物料名称
     */
    private String childName;
}
