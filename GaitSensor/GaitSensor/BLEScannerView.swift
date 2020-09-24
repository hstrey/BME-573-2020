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
            sensorListView(sensorList: viewmodel.blesensors)
            Spacer()
            Button(action: {viewmodel.scan()}, label: {Text("Scan")})
                .padding()
                .foregroundColor(Color.red)
        }
    }
    
    struct sensorListView: View {
        var sensorList: [BLESensor]
        var body: some View {
            NavigationView {
                List { ForEach(sensorList) {sensor in
                    Text(sensor.scannerName)
                    }
                }.navigationBarTitle("Sensors")
            }.padding()

        }
    }
}

//struct ContentView_Previews: PreviewProvider {
//    static var previews: some View {
//        BLEChooserView()
//    }
//}
