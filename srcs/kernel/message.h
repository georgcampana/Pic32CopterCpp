/* 
 * File:   message.h
 * Author: Georg Campana
 *
 * Created on 28 aprile 2013, 16.55
 */

#ifndef MESSAGE_H
#define	MESSAGE_H

#include "node.h"

class MsgPort;

class Message : public Node {
 public:
    
    Message(void* data, int len, MsgPort* reply_port = 0);
    Message(int data, MsgPort* reply_port = 0);
    Message(unsigned int data, MsgPort* reply_port = 0);

    unsigned int GetUIntPayload() const;
    int GetIntPayload() const;
    void* GetPayload() const;
    int GetDatalen() const;
    bool IsReplyRequested() const;

    void Reply(int result = 0);
    int GetReplyResult() const;


    friend class MsgPort;
 private:
    union {
        void* dataptr;
        int dataint;
        unsigned int datauint;
        int replyresult;
    } payload;
    int datalen;
    MsgPort* replyport;
    enum MsgStatus {
        MS_NONE = 0,
        MS_POSTED,
        MS_RETRIEVED,
        MS_REPLIED,
        MS_REPLYRETRIEVED
    } status;
    
    enum MsgType {
        MT_SIMPLE,
        MT_REPLYREQUESTED
    } type;
};

inline Message::Message(void* data, int len, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.dataptr = data;
    datalen = len;
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline Message::Message(int data, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.dataint = data;
    datalen = sizeof(int);
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline Message::Message(unsigned int data, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.datauint = data;
    datalen = sizeof(unsigned int);
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline unsigned int Message::GetUIntPayload() const { return payload.datauint; }

inline int Message::GetIntPayload() const { return payload.dataint; }
inline void* Message::GetPayload() const { return payload.dataptr; }
inline int Message::GetDatalen() const { return datalen; }
inline bool Message::IsReplyRequested() const { return(type == MT_REPLYREQUESTED); }

inline int Message::GetReplyResult() const { return payload.replyresult; }

#endif	/* MESSAGE_H */

