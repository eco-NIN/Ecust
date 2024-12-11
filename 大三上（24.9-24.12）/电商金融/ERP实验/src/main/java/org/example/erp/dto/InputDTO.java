package org.example.erp.dto;

import lombok.Data;

import java.time.LocalDate;

/**
 * @author hutianlin
 * 2024/10/10 16:23
 */
@Data
public class InputDTO {

    /**
     * 查询名称
     */
    private String name;

    /**
     * 需求量
     */
    private Integer demand;

    /**
     * 提货时间
     */
    private LocalDate completionDate;
}
