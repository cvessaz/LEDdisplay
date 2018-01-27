//
//  ViewController.swift
//  displayClient
//
//  Created by Christian Vessaz on 27.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
  
  //MARK: Properties
  @IBOutlet weak var messageTextField: UITextField!
  @IBOutlet weak var redSlider: UISlider!
  @IBOutlet weak var greenSlider: UISlider!
  @IBOutlet weak var blueSlider: UISlider!
  @IBOutlet weak var speedSlider: UISlider!
  @IBOutlet weak var outputTextField: UITextView!
  
  override func viewDidLoad() {
    super.viewDidLoad()
    // Do any additional setup after loading the view, typically from a nib.
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
    // Dispose of any resources that can be recreated.
  }

  //MARK: Actions
  @IBAction func stopEditingMessage(_ sender: UITextField) {
    sender.resignFirstResponder()
  }
  
  @IBAction func updateTintsColor(_ sender: UISlider) {
    let tintColor = UIColor(red: CGFloat(redSlider.value)/255.0,
                            green: CGFloat(greenSlider.value)/255.0,
                            blue: CGFloat(blueSlider.value)/255.0,
                            alpha: CGFloat(1.0))
    redSlider.maximumTrackTintColor = tintColor;
    greenSlider.maximumTrackTintColor = tintColor;
    blueSlider.maximumTrackTintColor = tintColor;
  }
  
  @IBAction func sendMessage(_ sender: UIButton) {
    stopEditingMessage(messageTextField)
    let cmd = "m/\(messageTextField.text!)"
    udpSend(textToSend: cmd)
  }
  
  @IBAction func sendColor(_ sender: UIButton) {
    let red = String(format: "%03d", Int(redSlider.value))
    let green = String(format: "%03d", Int(greenSlider.value))
    let blue = String(format: "%03d", Int(blueSlider.value))
    let cmd = "c/\(red)/\(green)/\(blue)"
    udpSend(textToSend: cmd)
  }
  
  @IBAction func sendSpeed(_ sender: UIButton) {
    let speed = String(format: "%03d", Int(speedSlider.value))
    let cmd = "s/\(speed)"
    udpSend(textToSend: cmd)
  }
  
  @IBAction func sendRestart(_ sender: UIButton) {
    let cmd = "r"
    udpSend(textToSend: cmd)
  }
  
  @IBAction func sendPause(_ sender: UIButton) {
    let cmd = "p"
    udpSend(textToSend: cmd)
  }
  
  func udpSend(textToSend: String) {
    print("Sending: \(textToSend)")
    outputTextField.text =  "\(textToSend)\n\n\(outputTextField.text!)"
    //outputTextField.layoutManager.allowsNonContiguousLayout = false
    //let stringLength:Int = outputTextField.text.count
    //outputTextField.scrollRangeToVisible(NSMakeRange(stringLength-1, 0))
    
    var address = in_addr()
    inet_pton(AF_INET, "127.0.0.1", &address)
    let port: CUnsignedShort = 1111
    
    func htons(value: CUnsignedShort) -> CUnsignedShort {
      return (value << 8) + (value >> 8);
    }
    
    let fd = socket(AF_INET, SOCK_DGRAM, 0) // DGRAM makes it UDP
    
    var addr = sockaddr_in(
      sin_len:    __uint8_t(MemoryLayout<sockaddr_in>.size),
      sin_family: sa_family_t(AF_INET),
      sin_port:   htons(value: port),
      sin_addr:   address,
      sin_zero:   ( 0, 0, 0, 0, 0, 0, 0, 0 )
    )
    
    textToSend.withCString { cstr -> Void in
      let sent: (Int) = withUnsafePointer(to: &addr) {
        let broadcastMessageLength = Int(strlen(cstr)+1)
        let p = UnsafeRawPointer($0).bindMemory(to: sockaddr.self, capacity: 1)
        return sendto(fd, cstr, broadcastMessageLength, 0, p, socklen_t(addr.sin_len))
      }
      print("Sent size: \(sent)")
    }
    
    close(fd)
  }

  

}

