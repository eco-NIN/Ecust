package org.example.erp.service.impl;

import jakarta.annotation.Resource;
import org.example.erp.exception.BusinessException;
import org.example.erp.mapper.FormulaMapper;
import org.example.erp.service.FormulaService;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * @author hutianlin
 * 2024/10/11 18:38
 */
@Service
public class FormulaServiceImpl implements FormulaService {

    @Resource
    private FormulaMapper formulaMapper;

    @Override
    public String generateFormula(String variableId) {
        String type = formulaMapper.getType(variableId);
        String id;
        List<String> formula;

        if ("assets".equals(type)) {
            id =  formulaMapper.getIdAssets(variableId);
            formula = formulaMapper.getFormulaAssets(id);
        }
        else if ("liabilities".equals(type)) {
            id = formulaMapper.getIdLiabilities(variableId);
            formula = formulaMapper.getFormulaLiabilities(id);
        }
        else {
            throw new BusinessException("无法识别的变量类型"+"'"+variableId+"'");
        }

        String answer = String.join("+", formula);
        return variableId + "=" + answer;

    }
}
