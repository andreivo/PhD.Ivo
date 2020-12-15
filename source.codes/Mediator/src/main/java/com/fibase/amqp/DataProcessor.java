/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fibase.amqp;

import com.fibase.Constants;
import com.fibase.database.WriteDataPackage;
import com.fibase.datapackage.DataPackage;
import com.fibase.mediator.DataPackageNormalizer;
import com.fibase.mqtt.MQTTProcessor;

/**
 *
 * @author andre
 */
public class DataProcessor {

    private final DataPackageNormalizer dpn;

    public DataProcessor() {
        this.dpn = new DataPackageNormalizer(Constants.ONTOLOGYPATH);
    }

    public void process(String message) {
        try {
            String normalizeDP = dpn.normalizeByOntology(message, DataPackage.class);
            DataPackage dp = DataPackage.fromJson(normalizeDP);
            if (dp != null) {
                if (dp.getTokenStation() != null) {
                    WriteDataPackage.csv(dp);
                    MQTTProcessor.publish(dp);
                }
            }
        } catch (Exception ex) {
            System.out.println("Error normalize DataPackage! Erro: " + ex.getMessage());
        }

    }

}
