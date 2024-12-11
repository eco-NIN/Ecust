package org.example.erp.mapper;

import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;

import java.util.List;

/**
 * @author hutianlin
 * 2024/10/11 16:56
 */
@Mapper
public interface FormulaMapper {

    /**
     * 获取变量类型
     */
    String getType(String id);

    /**
     * 资产类获取id
     */
    String getIdAssets(String id);

    /**
     * 负债类获取id
     */
    String getIdLiabilities(String id);

    /**
     * 资产类获取子节点id列表
     */
    List<String> getFormulaAssets(String id);

    /**
     * 负债类获取子节点id列表
     */
    List<String> getFormulaLiabilities(String id);
}
