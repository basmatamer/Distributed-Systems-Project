
#include <string>
#include <fstream>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include<QMessageBox>
#include <QFile>
#include <QTimer>

#include "globalvar.h"
#include "ui_login.h"
#include "profile.h"
#include "login.h"
#include "proxyFunctions.hpp"
#include "client.hpp"
#include "LocalDirectory.hpp"
#include "InterfaceDefintion.h"
#include "Configs.h"

#include "ApplicationServer.h"
#include "ourexception.h"

#include <thread>
#include <vector>
#include <string>

using namespace std;

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login) {

    std::cout << "Constructing the login screen" << std::endl;

    login_window = this;
    myClient = NULL;

//    locks_on_images.clear();

    setHomeDirectory();

    if(checkAppPath()) {
        std::cout << "The application path was succesfully found" << std::endl;
        if(checkDefImgPath()) {
            std::cout << "The default image was succesfully found" << std::endl;
            ui->setupUi(this);
        } else {
            std::cout << "The default image was NOT found" << std::endl;
            if (QMessageBox::Ok == QMessageBox::warning(this, "Error", "Default image not found\nContact us to resolve this issue")) {
                QTimer::singleShot(0, this, SLOT(close()));
            }
        }
    } else {
        std::cout << "The application path was NOT found" << std::endl;
        if (QMessageBox::Ok == QMessageBox::warning(this, "Error", "Application folder not found\nContact us to resolve this issue")) {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
}


Login::~Login() {
    std::cout << "ATTENTION ****************** We are now deleting the Login page" << std::endl;
    if(myClient != NULL) {
        delete myClient;
        myClient = NULL;
    }
    delete ui;
}

void Login::on_pushButton_login_clicked() {
    string password, directoryIP;

    user_username_string = ui->editline_username->text().toUtf8().constData();
    password = ui->editline_password->text().toUtf8().constData();
    ui->editline_password->setText("");
    directoryIP = ui->editline_directoryIP->text().toUtf8().constData();

    if(user_username_string.length() < 1 || user_username_string.find(" ") != std::string::npos || user_username_string.find("_") != std::string::npos) {
        QMessageBox::warning(this,"Invalid name", "No spaces or dashes in the username.\nUsername can't be empty");
        return;
    }
    else {
        if(password.length() < 1 || password.find(" ") != std::string::npos || password.find("_") != std::string::npos) {
            QMessageBox::warning(this,"Invalid password", "No spaces or dashes in the password.\nPassword can't be empty");
            return;
        } else {
            if(!checkDirectoryExists(defpath1)) {
                QMessageBox::warning(this,"Invalid name", "This user does not exist on this machine\nPlease log in from your machine");
                return;
            } else {

//                if(compareUsers(user_username_string, password))
//                {
//                    QMessageBox::information(this,"Valid Login", "logged in without internet");
//                    any_log = new logs ();
//                    hide();
//                    Profile * pf = new Profile();//myClient,this);
//                    pf->show();
//                }
//                else
//                {
//                    QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
//                }


                if(directoryIP.length() < 1 || directoryIP.find(" ") != std::string::npos || directoryIP.find("_") != std::string::npos) {
                    QMessageBox::warning(this,"Invalid directory IP", "No spaces or dashes in the directory IP.\Directory IP can't be empty");
                    return;
                }
                else {

                    InternetAddress my_address, my_server_address, directory_service_address;

                    my_address.ip = LocalDirectory::GetPrimaryIp();
                    my_address.port = CLIENT_PORT;

                    my_server_address.ip = LocalDirectory::GetPrimaryIp();
                    my_server_address.port = SERVER_PORT;

                    try {
                        directory_service_address.ip = LocalDirectory::IP_fromString(directoryIP);
                    } catch(const InvalidIPException& except) {
                        QMessageBox::warning(this,"Invalid directory IP", "No spaces or dashes in the directory IP.\Directory IP can't be empty");
                        return;
                    }
                    directory_service_address.port = DIRECTORY_PORT;

                    //disable login
                    ui->pushButton_login->setEnabled(false);
                    ui->pushButton_signup->setEnabled(false);
                    ui->editline_username->setEnabled(false);
                    ui->editline_password->setEnabled(false);
                    ui->editline_directoryIP->setEnabled(false);

                    try {
                        myClient = new Client(user_username_string, my_address, my_server_address, directory_service_address);

                    } catch(const SocketBindingFailedException& except) {
                        has_internet = false;

                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);


                        if(compareUsers(user_username_string, password)) {
                            QMessageBox::information(this, "Connection Error.", "Socket creation faied."
                                                                            "\nYou have been logged in through offline mode."
                                                                            "\nYou will have limited capabilities.");
                            myClient = NULL;
                            any_log = new logs ();
                            hide();
                            Profile * pf = new Profile();
                            pf->show();
                            return;

                        }
                        else
                        {
                            QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                            return;
                        }

                    } catch(const SocketCreationFailedException& except) {
                        has_internet = false;

                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);

                        if(compareUsers(user_username_string, password)) {
                            QMessageBox::information(this, "Connection Error.", "Socket creation faied."
                                                                            "\nYou have been logged in through offline mode."
                                                                            "\nYou will have limited capabilities.");
                            myClient = NULL;
                            any_log = new logs ();
                            hide();
                            Profile * pf = new Profile();
                            pf->show();
                            return;

                        }
                        else
                        {

                            QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                            return;
                        }
                    }

                    bool login_result;

                    try {
                        login_result = proxyFunctions::login(myClient, user_username_string, password);
                    } catch(const DirectoryServiceUnreachableException& except) {
                        has_internet = false;

                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);

                        delete myClient;

                        if(compareUsers(user_username_string, password)) {
                            QMessageBox::information(this, "Connection Error.", "Directory Service Unreachable."
                                                                            "\nYou have been logged in through offline mode."
                                                                            "\nYou will have limited capabilities.");
                            myClient = NULL;
                            any_log = new logs ();
                            hide();
                            Profile * pf = new Profile();
                            pf->show();
                            return;
                        }
                        else
                        {

                            QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                            return;
                        }

                    } catch(const UnknownUsernameException& except) {

                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);

                        delete myClient;

                        if(compareUsers(user_username_string, password)) {
                            QMessageBox::information(this, "Connection Error.", "Unknown Username"
                                                                            "\nYou have been logged in through offline mode."
                                                                            "\nYou will have limited capabilities.");
                            myClient = NULL;
                            any_log = new logs ();
                            hide();
                            Profile * pf = new Profile();
                            pf->show();
                        }
                        else
                        {

                            QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                            return;
                        }


                    } catch(const SendingFailedException& except) {
                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);

                        delete myClient;

                        if(compareUsers(user_username_string, password)) {
                            QMessageBox::information(this, "Connection Error.", "You seem to have lost internt access."
                                                                            "\nYou have been logged in through offline mode."
                                                                            "\nYou will have limited capabilities.");
                            myClient = NULL;
                            any_log = new logs ();
                            hide();
                            Profile * pf = new Profile();
                            pf->show();
                        }
                        else
                        {

                            QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                            return;
                        }

                    }



                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);



                    if(!login_result) {

                        QMessageBox::warning(this,"Invalid Login", "incorrect username or password");
                        delete myClient;
                        myClient = NULL;
                        has_internet = false;
                        return;
                    } else {

//                        std::vector<std::string> folders_people = GetDirectoryFiles(shared_images_path);
//                        for(unsigned int i = 0; i < folders_people.size(); i++) {
//                            std::vector<std::string> images_files = GetDirectoryFiles(shared_images_path + folders_people[i] + "/");
//                            for(unsigned int j = 0; j < images_files.size(); j++) {
//                                ImageReferenceMap new_img_ref_map;
//                                new_img_ref_map.owner_username = folders_people[i];
//                                new_img_ref_map.image_name = images_files[j].substr(images_files[j].find_last_of('_' + 1, images_files[j].length()));
//                                locks_on_images[new_img_ref_map] = new std::mutex();
//                            }

//                        }



                        any_log = new logs ();
                        has_internet = true;

                        std::thread server_thread(ApplicationServer::serverMain, my_server_address.ip, my_server_address.port);
                        server_thread.detach();



                        InternetAddress temp_s;
                        temp_s.ip = 0;
                        temp_s.port = 0;
                        std::thread robot_thread(proxyFunctions::roam_service_client, myClient->my_username, temp_s, myClient->my_server_address, myClient->directory_service_address, myClient->my_password);
                        robot_thread.detach();

                        // std::thread robot_thread(proxyFunctions::roam_service_client, statisticsLog, myClient->my_username, temp_s, myClient->my_server_address, myClient->directory_service_address, myClient->my_password);


                        proxyFunctions::hiServer(myClient, my_server_address);
                        QMessageBox::information(this,"WELCOME", "YOU ARE NOW LOGGED IN!");
                        hide();
                        Profile * pf = new Profile();//myClient,this);
                        pf->show();
                        return;

                    }
                }
            }
        }
    }
}

void Login::on_pushButton_signup_clicked() {
    string password, directoryIP, dis_msg;

    user_username_string = ui->editline_username->text().toUtf8().constData();
    password = ui->editline_password->text().toUtf8().constData();
    ui->editline_password->setText("");
    directoryIP = ui->editline_directoryIP->text().toUtf8().constData();

    if(user_username_string.length() < 1 || user_username_string.find(" ") != std::string::npos || user_username_string.find("_") != std::string::npos) {
        QMessageBox::warning(this,"Invalid name", "No spaces or dashes in the username.\nUsername can't be empty");
        return;
    }
    else {
        if(password.length() < 1 || password.find(" ") != std::string::npos || password.find("_") != std::string::npos) {
            QMessageBox::warning(this,"Invalid password", "No spaces or dashes in the password.\nPassword can't be empty");
            return;
        } else {
            if(directoryIP.length() < 1 || directoryIP.find(" ") != std::string::npos || directoryIP.find("_") != std::string::npos) {
                QMessageBox::warning(this,"Invalid directory IP", "No spaces or dashes in the directory IP.\Directory IP can't be empty");
                return;
            }
            else
            {


                InternetAddress my_address, my_server_address, directory_service_address;

                my_address.ip = LocalDirectory::GetPrimaryIp();
                my_address.port = CLIENT_PORT;

                my_server_address.ip = LocalDirectory::GetPrimaryIp();
                my_server_address.port = SERVER_PORT;

                try {
                    directory_service_address.ip = LocalDirectory::IP_fromString(directoryIP);
                } catch(const InvalidIPException& except) {
                    QMessageBox::warning(this,"Invalid directory IP", "No spaces or dashes in the directory IP.\Directory IP can't be empty");
                    return;
                }
                directory_service_address.port = DIRECTORY_PORT;

                myClient = new Client(user_username_string, my_address, my_server_address, directory_service_address);

                //disable
                ui->pushButton_login->setEnabled(false);
                ui->pushButton_signup->setEnabled(false);
                ui->editline_username->setEnabled(false);
                ui->editline_password->setEnabled(false);
                ui->editline_directoryIP->setEnabled(false);

                try {
                    if(!proxyFunctions::signup(myClient, user_username_string, password, dis_msg)) {
                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);

                        QMessageBox::warning(this,"Invalid SignUp", QString::fromStdString(dis_msg));
                        delete myClient;
                        myClient = NULL;
                        return;
                    }
                    else {
                        ui->pushButton_login->setEnabled(true);
                        ui->pushButton_signup->setEnabled(true);
                        ui->editline_username->setEnabled(true);
                        ui->editline_password->setEnabled(true);
                        ui->editline_directoryIP->setEnabled(true);
                        createUserFolder();

                        any_log = new logs ();
                        has_internet = true;

                        std::thread server_thread(ApplicationServer::serverMain, my_server_address.ip, my_server_address.port);
                        server_thread.detach();


                        InternetAddress temp_s;
                        temp_s.ip = 0;
                        temp_s.port = 0;
                        std::thread robot_thread(proxyFunctions::roam_service_client,
                                                 myClient->my_username, temp_s, myClient->my_server_address,
                                                 myClient->directory_service_address, myClient->my_password);
                        robot_thread.detach();

                        proxyFunctions::hiServer(myClient, my_server_address);

                        std::ofstream output;
                        output.open(SIGNED_UP_FILENAME, std::ios_base::app);
                        if(output.is_open())
                        {
                            output << encrypt(user_username_string) << " " << encrypt(password) << std::endl;
                            output.close();
                        }
                        else
                        {
                            std::cout<< "Couldn't open file" << std::endl;
                        }


                        QMessageBox::information(this,"WELCOME", QString::fromStdString(dis_msg));
                        hide();
                        Profile * pf = new Profile();
                        pf->show();
                        return;

                    }

                } catch(const DirectoryServiceUnreachableException& except) {
                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);
                    QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

                    delete myClient;
                    return;


                } catch(const UnknownUsernameException& except) {
                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);
                    QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

                    delete myClient;
                    return;


                } catch(const SendingFailedException& except) {
                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);
                    QMessageBox::warning(this, "Connection Error.", "It seems you don't have internet access."
                                                                    "\nYou need Internet access to signup."
                                                                    "\nGoodbye.");

                    QCoreApplication::quit();
                } catch(const SocketBindingFailedException& except) {
                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);
                    QMessageBox::warning(this, "Connection Error.", "Socket creation faied."
                                                                    "\nYou need a socket to signup."
                                                                    "\nGoodbye.");

                    QCoreApplication::quit();
                } catch(const SocketCreationFailedException& except) {
                    ui->pushButton_login->setEnabled(true);
                    ui->pushButton_signup->setEnabled(true);
                    ui->editline_username->setEnabled(true);
                    ui->editline_password->setEnabled(true);
                    ui->editline_directoryIP->setEnabled(true);
                    QMessageBox::warning(this, "Connection Error.", "Socket binding failed."
                                                                    "\nYou need a socket to signup."
                                                                    "\nGoodbye.");

                    QCoreApplication::quit();
                }
          }
        }
    }
}


