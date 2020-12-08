/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fibase.amqpexample;

import com.fibase.datapackage.DataPackage;
import java.time.LocalDateTime;

/**
 *
 * @author andre
 */
public class MainDataPackage {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        DataPackage dp = new DataPackage();
        dp.setTokenStation("TestStation");
        dp.setPasswdStation("passwdTest");
        dp.setSentDateTime(LocalDateTime.now());
        dp.addMeasures(LocalDateTime.now(), "20.4", 20L, "pluvio");
        dp.addMeasures(LocalDateTime.now(), "21.4", 21L, "battery");

        String message = dp.toJson();

        System.out.println(message);
    }

}
