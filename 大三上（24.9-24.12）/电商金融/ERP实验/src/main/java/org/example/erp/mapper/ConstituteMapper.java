package org.example.erp.mapper;

import org.apache.ibatis.annotations.Mapper;
import org.example.erp.entity.Constitute;

import java.util.List;

/**
 * @author hutianlin
 * 2024/10/11 00:58
 */
@Mapper
public interface ConstituteMapper {

    List<Constitute> selectItemById(String id);

    Integer getIngredientsLeadTime(String id);

    Integer getSupplyLeadTime(String id);
}
