//
//  BLEScanners.swift
//  GaitSensor
//
//  Created by Helmut Strey on 9/21/20.
//  Copyright © 2020 Helmut Strey. All rights reserved.
//

import Foundation

struct BLESensors {
    var availSensors: [BLESensor] = []
    
    mutating func addscanner(scannerName:String,rssi:Int){
        availSensors.append(BLESensor(scannerName:scannerName,rssi:rssi))
    }
}

struct BLESensor: Identifiable {
    var scannerName: String
    var rssi: Int
    var id : UUID = UUID()
}
