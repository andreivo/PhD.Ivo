/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fibase.mqtt;

import static com.fibase.QueueIdentity.MQTTBROKERURL;
import static com.fibase.QueueIdentity.MQTTTOKEN;
import static com.fibase.QueueIdentity.MQTTTOPIC;
import com.fibase.datapackage.DataPackage;
import com.fibase.datapackage.MeasuresDtp;
import java.io.UnsupportedEncodingException;
import java.time.format.DateTimeFormatter;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 *
 * @author andre
 */
public class MQTTProcessor {

    public static void publish(DataPackage dp) throws MqttException, UnsupportedEncodingException {

        System.out.println("Publishing data to MQTTServer....");

        // Data setup
        String payload = formatPayload(dp);
        int qos = 0;

        // Creates the MQTT client
        MqttClient client;
        MqttConnectOptions conOpt;

        conOpt = new MqttConnectOptions();
        conOpt.setCleanSession(true);
        conOpt.setUserName(MQTTTOKEN);

        client = new MqttClient(MQTTBROKERURL, MqttClient.generateClientId());

        if (!client.isConnected()) {
            System.out.println("Connecting to MQTT broker ...");
            client.connect(conOpt);
        }

        if (client.isConnected()) {
            System.out.println("Send MQTT message....");

            // Sends Data
            MqttMessage message = new MqttMessage(payload.getBytes("UTF-8"));
            message.setQos(qos);

            client.publish(MQTTTOPIC + dp.getTokenStation().toLowerCase(), message);
            client.disconnect();
        }
    }

    private static String formatPayload(DataPackage dp) {
        String result = "";

        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");

        for (MeasuresDtp measure : dp.getMeasures()) {
            if (result != "") {
                result = result + ",";
            }
            result = result + "\"" + measure.getDataType() + "\":";
            //result = result + measure.getDataValue();
            result = result + "{\"value\":" + measure.getDataValue();
            result = result + ",\"context\":{\"collect\":\"" + measure.getCollectDateTime().format(formatter) + "\"}}";
        }

        //{"VARIABLE_LABEL" : {"value" : 30 , "context" : {"key" : "KEY-VALUE"}}}
        //{"pluvio"         : {"value" : 24 , "context" : {"collect":"2020-12-06 18:10:24"}},"battery":{"value":143,99977,"context":{"collect":"2020-12-06 18:10:24"}}}
        System.out.println("{" + result + "}");

        return "{" + result + "}";
    }

}
