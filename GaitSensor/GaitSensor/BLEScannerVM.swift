//
//  BLEScannerVM.swift
//  GaitSensor
//
//  Created by Helmut Strey on 9/23/20.
//  Copyright © 2020 Helmut Strey. All rights reserved.
//

import SwiftUI
import CoreBluetooth

class BLEScannerVM: NSObject, ObservableObject, CBCentralManagerDelegate {
    
    @Published private var scannerdata: BLESensors = createBLESensors()
    var centralManager : CBCentralManager!
    
    static func createBLESensors() -> BLESensors {
        return BLESensors()
    }
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    // MARK: - Access to data
    var blesensors: [BLESensor] {
        return scannerdata.availSensors
    }
    
    // MARK: - Intents
    func scan() {
        print("scanning...")
        if centralManager.state == .poweredOn {
            centralManager.scanForPeripherals(withServices: [], options: nil)
        }
    }
    
    // MARK: - Bluetooth
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print("DidUpdateState called")
        if central.state == .poweredOn {
            print("BLE working")
        } else {
            print("ERROR: BLE is not working")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print("\(peripheral.name ?? "nothing") rssi:\(RSSI)")
        scannerdata.addscanner(scannerName: peripheral.name ?? "noname", rssi: Int(truncating: RSSI))
    }
}
