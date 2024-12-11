package org.example.erp.utils;

import org.example.erp.dto.ResultDTO;

import java.util.Comparator;

/**
 * @author hutianlin
 * 2024/10/11 10:03
 * ResultDTO比较器
 */
public class ResultDTOComparator implements Comparator<ResultDTO> {
    public int compare(ResultDTO o1, ResultDTO o2) {
        return o2.getCompletionDate().compareTo(o1.getCompletionDate());
    }
}
