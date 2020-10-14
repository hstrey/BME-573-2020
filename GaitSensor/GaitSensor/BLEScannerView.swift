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
            VStack{
                Text("\(sensor.scannerName)")
                dataDisplay(sensor:sensor, vm:vm)
            }.onAppear() {
                vm.connect(peripheral:sensor)
            }
            .onDisappear() {
                vm.disconnect(peripheral: sensor)
            }
        }
    }
    
    struct dataDisplay: View {
        var sensor:BLESensor
        var vm:BLEScannerVM
        var body: some View {
            VStack{
                GeometryReader { geometry in
                    ZStack{
                        Rectangle().fill(Color(UIColor.systemBackground)).border(Color.white, width: /*@START_MENU_TOKEN@*/1/*@END_MENU_TOKEN@*/)
                        
                        Path { path in
                            path.move(to: CGPoint(x: 0, y: geometry.size.height/2))
                            path.addLine(to: CGPoint(x: geometry.size.width,y: geometry.size.height/2))
                        }.stroke().foregroundColor(.yellow)
                        
                        Path { path in
                            let data = vm.bledata(uuid: sensor.id)
                            let width = Int(geometry.size.width)
                            let lengthData = data.count
                            var start = data.count - width
                            if start < 0 {
                                start = 0
                            }
                            if lengthData>start {
                            path.move(to: CGPoint(x: 0.0, y: geometry.size.height*(1.0-CGFloat(data[start])/4096)))
                            for i in start..<lengthData {
                                path.addLine(to: CGPoint(x: CGFloat(i-start),
                                                         y: geometry.size.height*(1.0-CGFloat(data[i])/4096)))
                            }}
                        }.stroke().foregroundColor(/*@START_MENU_TOKEN@*/.blue/*@END_MENU_TOKEN@*/)
                    }
                }
            }
        }
    }
}

//struct ContentView_Previews: PreviewProvider {
//    static var previews: some View {
//        dataDisplay()
//            .preferredColorScheme(.dark)
//    }
//}
