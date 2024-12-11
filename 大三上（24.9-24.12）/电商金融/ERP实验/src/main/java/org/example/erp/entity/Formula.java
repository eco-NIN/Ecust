package org.example.erp.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author hutianlin
 * 2024/10/11 16:50
 * 资产负债表计算公式定义
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class Formula {

    /**
     * 表序号
     */
    private String id;

    /**
     * 资产类序号
     */
    private String assetsSummaryId;

    /**
     * 负债类序号
     */
    private String liabilitiesSummaryId;

    /**
     * 资产类变量名
     */
    private String assetsId;

    /**
     * 负债类变量名
     */
    private String liabilitiesId;
}
