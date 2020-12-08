/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fibase.amqpexample;

import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Consumer;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;
import java.io.IOException;

public class MainReceiveMessage {

    private static final String EXCHANGE_NAME = "fibase";
    private final static String QUEUE_NAME = "DCPPluvio";
    private final static String ROUTINGKEY = "DCPRouting";

    public static void main(String[] argv) throws Exception {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setUsername("tcsezysb");
        factory.setVirtualHost("tcsezysb");
        factory.setPassword("7dUNxiB9DCQgvcKkon-sDcDZIVhkCtFN");
        factory.setHost("jackal.rmq.cloudamqp.com");

        Connection connection = factory.newConnection();
        Channel channel = connection.createChannel();

        //DeclareQueue: Ensuring that the queue exists
        channel.queueDeclare(QUEUE_NAME, true, false, false, null);
        //Defines the key routes that forward messages to the queue
        channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ROUTINGKEY);

        //Defines Exchange, responsible for distributing messages in the queues
        channel.exchangeDeclare(EXCHANGE_NAME, "direct", true);

        System.out.println(" [*] Waiting for messages. To exit press CTRL+C");

        Consumer consumer = new DefaultConsumer(channel) {
            public void handleDelivery(String consumerTag, Envelope envelope,
                    AMQP.BasicProperties properties, byte[] body)
                    throws IOException {
                String message = new String(body, "UTF-8");
                System.out.println(" Received " + envelope.getRoutingKey() + ": '" + message + "'");
            }
        };

        channel.basicConsume(QUEUE_NAME, true, consumer);

    }
}
