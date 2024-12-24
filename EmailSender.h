#ifndef EmailSender_h
#define EmailSender_h

#include "Arduino.h"
#include <ESP_Mail_Client.h>

class EmailSender {
  public:
    EmailSender(String smtp_host, int smtp_port, String smtp_sender_email, String smtp_sender_password, String recipient_email);
    void SetRecipientEmail(String recipient_email);
    void SendEmail(String msgToBeSent);
  private:
//    void smtpCallback(SMTP_Status status);
    String SMTP_HOST;
    int SMTP_PORT;
    String SMTP_SENDER_EMAIL;
    String SMTP_SENDER_PASSWORD;
    String RECIPIENT_EMAIL;
    SMTPSession smtp;
    Session_Config config;
    SMTP_Message message;
};

#endif
