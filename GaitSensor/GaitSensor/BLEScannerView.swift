//
//  BLEScannerView.swift
//  GaitSensor
//
//  Created by Helmut Strey on 9/21/20.
//  Copyright © 2020 Helmut Strey. All rights reserved.
//

import SwiftUI

struct BLEScannerView: View {
    @ObservedObject var viewmodel : BLEScannerVM
    var body: some View {
        VStack {
            sensorListView(sensorList: viewmodel.blesensors,vm:viewmodel)
            Spacer()
            Button(action: {viewmodel.scan()}, label: {Text("Scan")})
                .padding()
                .foregroundColor(Color.red)
        }
    }
    
    struct sensorListView: View {
        var sensorList: [BLESensor]
        var vm:BLEScannerVM
        var body: some View {
            NavigationView {
                List { ForEach(sensorList) {sensor in
                    NavigationLink(destination: dataView(sensor:sensor,vm:vm)){
                        VStack(alignment:.leading){
                            Text("\(sensor.scannerName) : \(sensor.rssi)")
                            Text("UUID: \(sensor.id)").font(.caption)
                        }
                    }
                }
                }.navigationBarTitle("Sensors")
                .onDisappear() {
                    print("navigationView disappeared")
                }
            }.padding()
        }
    }
    
    struct dataView: View {
        var sensor:BLESensor
        var vm:BLEScannerVM
        var body: some View {
            Text("\(sensor.scannerName)").onAppear() {
                vm.connect(peripheral:sensor)
            }
            .onDisappear() {
                vm.disconnect(peripheral: sensor)
            }
        }
    }
}

//struct ContentView_Previews: PreviewProvider {
//    static var previews: some View {
//        BLEChooserView()
//    }
//}
