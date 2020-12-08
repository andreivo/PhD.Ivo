/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fibase.amqpexample;

import com.fibase.datapackage.DataPackage;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import java.time.LocalDateTime;

/**
 *
 * @author andre
 */
public class MainSendMessage {

    private static final String EXCHANGE_NAME = "fibase";
    private final static String QUEUE_NAME = "DCPPluvio";
    private final static String ROUTINGKEY = "DCPRouting";

    public static void main(String[] argv) throws Exception {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setUsername("tcsezysb");
        factory.setVirtualHost("tcsezysb");
        factory.setPassword("7dUNxiB9DCQgvcKkon-sDcDZIVhkCtFN");
        factory.setHost("jackal.rmq.cloudamqp.com");

        try (Connection connection = factory.newConnection();
                Channel channel = connection.createChannel()) {
            //DeclareQueue: Ensuring that the queue exists
            channel.queueDeclare(QUEUE_NAME, true, false, false, null);

            //Defines Exchange, responsible for distributing messages in the queues
            channel.exchangeDeclare(EXCHANGE_NAME, "direct", true);

            //Defines the key routes that forward messages to the queue
            channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ROUTINGKEY);

            DataPackage dp = new DataPackage();
            dp.setTokenStation("TestStation");
            dp.setPasswdStation("passwdTest");
            dp.addMeasures(LocalDateTime.now(), "20.4", 20L, "pluvio");

            String message = dp.toJson();

            channel.basicPublish(EXCHANGE_NAME, ROUTINGKEY, null, message.getBytes("UTF-8"));
            System.out.println(" [x] Sent '" + message + "'");

        }
    }

}
