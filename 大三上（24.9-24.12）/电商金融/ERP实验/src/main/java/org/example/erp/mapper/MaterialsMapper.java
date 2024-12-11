package org.example.erp.mapper;

import org.apache.ibatis.annotations.Mapper;

/**
 * @author hutianlin
 * 2024/10/11 01:46
 */
@Mapper
public interface MaterialsMapper {

    String getMaterialsMethod(String id);

    Integer getMaterialsOperationLeadTime(String id);

    Double getLossRate(String id);

    String getIdByName(String name);
}
