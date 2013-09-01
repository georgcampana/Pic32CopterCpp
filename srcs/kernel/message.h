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
    
    Message(void* data, Int32 len, MsgPort* reply_port = 0);
    Message(Int32 data, MsgPort* reply_port = 0);
    Message(UInt32 data, MsgPort* reply_port = 0);
    Message(int data, MsgPort* reply_port = 0);

    UInt32 GetUIntPayload() const;
    Int32 GetIntPayload() const;
    void* GetPayload() const;
    Int32 GetDatalen() const;
    bool IsReplyRequested() const;

    void Reply(Int32 result = 0);
    Int32 GetReplyResult() const;


    friend class MsgPort;
 private:
    union {
        void* dataptr;
        Int32 dataint;
        UInt32 datauint;
        Int32 replyresult;
    } payload;
    Int32 datalen;
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

inline Message::Message(void* data, Int32 len, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.dataptr = data;
    datalen = len;
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline Message::Message(Int32 data, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.dataint = data;
    datalen = sizeof(Int32);
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline Message::Message(int data, MsgPort* reply_port) { Message((Int32) data, reply_port); }

inline Message::Message(UInt32 data, MsgPort* reply_port) : Node(NT_MSG), status(MS_NONE),type(MT_SIMPLE), replyport(reply_port) {
    this->payload.datauint = data;
    datalen = sizeof(UInt32);
    if(reply_port) { type=MT_REPLYREQUESTED; }
}

inline UInt32 Message::GetUIntPayload() const { return payload.datauint; }

inline Int32 Message::GetIntPayload() const { return payload.dataint; }
inline void* Message::GetPayload() const { return payload.dataptr; }
inline Int32 Message::GetDatalen() const { return datalen; }
inline bool Message::IsReplyRequested() const { return(type == MT_REPLYREQUESTED); }

inline Int32 Message::GetReplyResult() const { return payload.replyresult; }

#endif	/* MESSAGE_H */

