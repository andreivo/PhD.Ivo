/*
 * *****************************************************************************
 *  Ministério da Ciência, Tecnologia, Inovações e Comunicações (MCTIC)
 *  Centro Nacional de Monitoramento e Alertas de Desastres Naturais (CEMADEN)
 *
 *  Este documento contém informações confidenciais e está protegido pela legis-
 *  lação brasileira (Constituição Federal, Art. 5º, inciso XII; Lei 9.296/1996,
 *  Art. 10; Lei 12.965/2014 Art. 7º, incisos I, II e III). Seu conteúdo é diri-
 *  gido ao uso do indivíduo ou da entidade mencionados acima.
 *
 *  This document contains confidential information protected by Brazilian laws
 *  and intended only for the use of the individual or entity named above.
 *
 * *****************************************************************************
 */
package com.fibase.datapackage;

import java.time.LocalDateTime;

public class MeasuresDtp {

    private Long sensorExternalCode;
    private String dateType;
    private LocalDateTime collectDateTime;
    private String value;

    public MeasuresDtp() {
    }

    public MeasuresDtp(LocalDateTime collectDateTime, String value, Long sensorExternalCode, String dateType) {
        this.collectDateTime = collectDateTime;
        this.value = value;
        this.sensorExternalCode = sensorExternalCode;
        this.dateType = dateType;
    }

    public LocalDateTime getCollectDateTime() {
        return collectDateTime;
    }

    public void setCollectDateTime(LocalDateTime collectDateTime) {
        this.collectDateTime = collectDateTime;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public Long getSensorExternalCode() {
        return sensorExternalCode;
    }

    public void setSensorExternalCode(Long sensorExternalCode) {
        this.sensorExternalCode = sensorExternalCode;
    }

    public String getDateType() {
        return dateType;
    }

    public void setDateType(String dateType) {
        this.dateType = dateType;
    }

}
