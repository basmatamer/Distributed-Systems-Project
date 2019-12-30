#include <stdio.h>
#include "InterfaceDefintion.h"
#include "ErrorHandler.hpp"

MessageTargetType getTargetType(OperationID op_id) {
    switch (op_id) {
        case RESERVED:
            return UNKOWN;
        case EMPTY:
            return UNKOWN;
        case TEXT_MESSAGE:
            return UNKOWN;
        case SIGNUP:
            return DIRECTORY_SERVICE;
        case LOGIN:
            return DIRECTORY_SERVICE;
        case HI_SERVER:
            return DIRECTORY_SERVICE;
        case GET_ADDRESS_OF_USER:
            return DIRECTORY_SERVICE;
        case CHANGE_PASSWORD:
            return DIRECTORY_SERVICE;
        case OFFER_SERVICE:
            return DIRECTORY_SERVICE;
        case GET_AVAILABLE_SERVICES:
            return DIRECTORY_SERVICE;
        case GET_USERNAMES:
            return DIRECTORY_SERVICE;

        case UPDATE_VIEWS_COUNTER:
            return ANOTHER_PEER;

        case REQUEST_IMAGE:
            return ANOTHER_PEER;
        case SEND_IMAGE:
            return ANOTHER_PEER;
        case REQUEST_ADDITIONAL_VIEWS:
            return ANOTHER_PEER;
        case RETREIVE_STATISTICS:
            return ANOTHER_PEER;

        case SUBMIT_PENDING_QUOTA_CHANGE_REQUEST:
            return DIRECTORY_SERVICE;
        case REMOVE_PENDING_QUOTA_CHANGES_REQUEST:
            return DIRECTORY_SERVICE;
        case TURN_OFF_DIRECTORY_SERVICE:
            return DIRECTORY_SERVICE;
        case LOGOUT:
            return MYSELF;
        case ANNOUNCE_IMAGE:
            return DIRECTORY_SERVICE;
       // case UPDATE_VIEWS_COUNTER:
         //   return ANOTHER_PEER;
        default:
            ErrorHandler::die("Unkwon operation id");
            return UNKOWN;

    }
}
