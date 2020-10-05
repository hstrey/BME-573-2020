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
            centralManager.scanForPeripherals(withServices: [pressure_service_uuid], options: nil)
            DispatchQueue.main.asyncAfter(deadline: .now() + 1.5) {
                print("stop scanning")
                self.centralManager.stopScan()
            }
        }
    }
    
    func connect(peripheral: BLESensor) {
        print("called connect...")
//        centralManager.connect(peripheral.peripheral, options:nil)
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
        print("local name: \(advertisementData["kCBAdvDataLocalName"] ?? "no local name")")
        print("uuid: \(peripheral.identifier)")
        scannerdata.addscanner(scannerName: peripheral.name ?? "noname",
                                   localName: (advertisementData["kCBAdvDataLocalName"] ?? "no local name") as! String,
                                    rssi: Int(truncating: RSSI),
                                    peripheral:peripheral,
                                    uuid: peripheral.identifier)
    }
    
    let pressure_char_uuid = CBUUID(string:"5A8AC39C-0CE1-4212-8B46-D589BA126CE2")
    let period_char_uuid = CBUUID(string:"F8BC0798-E447-4625-959F-CE6970B70ED1")
    let pressure_service_uuid = CBUUID(string:"573BC605-1C3C-4467-B4AA-44E0C6C6B410")

}
