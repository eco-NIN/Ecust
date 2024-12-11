package org.example.erp.service.impl;

import jakarta.annotation.Resource;
import org.example.erp.entity.Constitute;
import org.example.erp.entity.TreeNode;
import org.example.erp.exception.BusinessException;
import org.example.erp.mapper.ConstituteMapper;
import org.example.erp.mapper.MaterialsMapper;
import org.example.erp.service.TreeBuildingService;
import org.springframework.stereotype.Service;

import java.time.LocalDate;
import java.util.List;

/**
 * @author hutianlin
 * 2024/10/11 00:50
 * 树的构建服务实现
 */
@Service
public class TreeBuildingServiceImpl implements TreeBuildingService {

    @Resource
    private ConstituteMapper constituteMapper;

    @Resource
    private MaterialsMapper materialsMapper;

    /**
     * 由节点构建树
     */
    @Override
    public void buildTree(TreeNode node) {

        List<Constitute> constituteList = constituteMapper.selectItemById(node.getId());

        for (Constitute constitute : constituteList) {
            TreeNode child = new TreeNode();
            Integer minusDays = 0;
            String id = constitute.getChildId();
            String method = materialsMapper.getMaterialsMethod(id);
            if ("1".equals(method)) {
                minusDays = materialsMapper.getMaterialsOperationLeadTime(id);
            }
            else if ("0".equals(method)) {
                minusDays = constitute.getIngredientsLeadTime() + constitute.getSupplyLeadTime();
            }


            child.setId(id);
            child.setParentId(constitute.getParentId());
            child.setName(constitute.getChildName());
            child.setCompletionDate(node.getOrderDate());
            child.setMethod(method);
            child.setDemand((int) (Math.ceil(node.getDemand() * constitute.getNumber() / (1 - materialsMapper.getLossRate(id)))));
            child.setOrderDate(child.getCompletionDate().minusDays(minusDays));
            child.setNumber(constitute.getNumber());

            buildTree(child);
            node.getChildren().add(child);
        }
    }

    /**
     * 初始化根节点
     */
    @Override
    public TreeNode setupRoot(String name, Integer demand, LocalDate completionDate) {
        TreeNode root = new TreeNode();
        String id = materialsMapper.getIdByName(name);
        String method = materialsMapper.getMaterialsMethod(id);

        if (id == null) {
            throw new BusinessException("物料名称"+"'"+name+"'"+"不存在");
        }

        root.setId(id);
        root.setParentId(id);
        root.setName(name);
        root.setDemand(demand);
        root.setCompletionDate(completionDate);
        root.setMethod(method);
        root.setNumber(1);
        root.setOrderDate(completionDate.minusDays(materialsMapper.getMaterialsOperationLeadTime(id)));

        return root;
    }

    @Override
    public void reBuildTree(TreeNode node) {

        if (node.getChildren() == null || node.getDemand() == null || node.getDemand() <= 0) {
            return;
        }

        for (TreeNode child : node.getChildren()) {
            child.setDemand((int) (Math.ceil(node.getDemand() * child.getNumber() / (1 - materialsMapper.getLossRate(child.getId())))));
            reBuildTree(child);
        }
    }
}
