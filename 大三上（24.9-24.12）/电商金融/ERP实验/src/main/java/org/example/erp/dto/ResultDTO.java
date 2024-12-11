package org.example.erp.dto;

import lombok.Data;

import java.time.LocalDate;

/**
 * @author hutianlin
 * 2024/10/10 16:35
 * 结果传输对象
 */
@Data
public class ResultDTO {

    /**
     * 物料号
     */
    private String ID;

    /**
     * 物料名称
     */
    private String name;

    /**
     * 调配方式
     */
    private String method;

    /**
     * 需求量
     */
    private Integer demand;

    /**
     * 日程下达日期
     */
    private LocalDate orderDate;

    /**
     * 日程完成日期
     */
    private LocalDate completionDate;
}
