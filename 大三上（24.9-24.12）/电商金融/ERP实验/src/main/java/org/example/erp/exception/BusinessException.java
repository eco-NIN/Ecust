package org.example.erp.exception;

import lombok.Data;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

/**
 * @author hutianlin
 * 2024/10/11 03:06
 */
@Getter
@Setter
@NoArgsConstructor
public class BusinessException extends RuntimeException {
    private String message;

    public BusinessException(String message) {
        super(message);
        this.message = message;
    }

}
