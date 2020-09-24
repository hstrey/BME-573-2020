//
//  BLEScannerView.swift
//  GaitSensor
//
//  Created by Helmut Strey on 9/21/20.
//  Copyright © 2020 Helmut Strey. All rights reserved.
//

import SwiftUI

struct BLEScannerView: View {
    var body: some View {
        VStack {
            sensorList()
            Spacer()
            Button(action: {}, label:{Text("Scan")})
                .padding()
                .foregroundColor(Color.red)
        }
    }
}

struct sensorList: View {
    var scannerNames: [String] = ["Pressure Sensor 1","Pressure Sensor 2"]
    var body: some View {
        NavigationView {
            List{
                ForEach(scannerNames, id: \.self) {sensor in
                    Text(sensor)
                }
            }.navigationBarTitle("Available Sensors")
        }
    }
}







































//struct ContentView_Previews: PreviewProvider {
//    static var previews: some View {
//        ContentView()
//    }
//}
