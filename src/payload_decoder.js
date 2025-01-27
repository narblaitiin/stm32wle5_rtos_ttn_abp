/*
 * Copyright (c) 2024
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

function Decoder(bytes, port) {
    var decoded = {};

    decoded.Battery = ((bytes[0] << 8) | bytes[1])/100

    if(bytes[2] === 0){
        decoded.Temperature = ((bytes[3] << 8) | bytes[4])/100
    }else{
        decoded.Temperature = (((bytes[3] << 8)| bytes[4]) * -1)/100
    }

    decoded.Humidity = ((bytes[5] << 8) | bytes[6])/100
    
    return decoded;
  }