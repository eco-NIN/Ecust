package org.example.erp.dto;

import lombok.Data;
import org.example.erp.utils.ResultDTOComparator;

import java.util.PriorityQueue;
import java.util.Queue;

/**
 * @author hutianlin
 * 2024/10/11 13:54
 * 结果队列
 */
@Data
public class ResultQueueDTO {
    private Queue<ResultDTO> results = new PriorityQueue<>(new ResultDTOComparator());
}
