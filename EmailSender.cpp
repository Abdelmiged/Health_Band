#include "Arduino.h"
#include "EmailSender.h"

EmailSender::EmailSender(String smtp_host, int smtp_port, String smtp_sender_email, String smtp_sender_password, String recipient_email) {
  SMTP_HOST = smtp_host;
  SMTP_PORT = 465;
  SMTP_SENDER_EMAIL = smtp_sender_email;
  SMTP_SENDER_PASSWORD = smtp_sender_password;
  RECIPIENT_EMAIL = recipient_email;
}

void EmailSender::SetRecipientEmail(String recipient_email) {
  RECIPIENT_EMAIL = recipient_email;
}

void EmailSender::SendEmail(String msgToBeSent) {
  MailClient.networkReconnect(true);
  smtp.debug(0);
//  smtp.keepAlive(60, 2, 1);
//  smtp.callback(smtpCallback);

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = SMTP_SENDER_EMAIL;
  config.login.password = SMTP_SENDER_PASSWORD;
  config.login.user_domain = "";

  message.sender.name = "ESP32";
  message.sender.email = SMTP_SENDER_EMAIL;
  message.subject = "ESP32 sending test";
  message.addRecipient("Gedo", RECIPIENT_EMAIL);

  String msg = msgToBeSent;
  message.text.content = msg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
}


//void EmailSender::smtpCallback(SMTP_Status status){
//  /* Print the current status */
//  Serial.println(status.info());
//
//  /* Print the sending result */
//  if (status.success()){
//    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
//    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
//    // In ESP8266 and ESP32, you can use Serial.printf directly.
//
//    Serial.println("----------------");
//    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
//    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
//    Serial.println("----------------\n");
//
//    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
//    {
//      /* Get the result item */
//      SMTP_Result result = smtp.sendingResult.getItem(i);
//
//      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
//      // your device time was synched with NTP server.
//      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
//      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
//      
//      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
//      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
//      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
//      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
//      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
//    }
//    Serial.println("----------------\n");
//
//    // You need to clear sending result as the memory usage will grow up.
//    smtp.sendingResult.clear();
//  }
//}
