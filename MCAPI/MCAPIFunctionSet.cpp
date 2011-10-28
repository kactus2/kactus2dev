//-----------------------------------------------------------------------------
// File: MCAPIFunctionSet.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.1.2011
//
// Description:
// MCAPI function set.
//-----------------------------------------------------------------------------

#include "MCAPIFunctionSet.h"

//-----------------------------------------------------------------------------
// Function: MCAPIFunctionSet()
//-----------------------------------------------------------------------------
MCAPIFunctionSet::MCAPIFunctionSet() : m_functions()
{
    buildFunctionList();
}

//-----------------------------------------------------------------------------
// Function: ~MCAPIFunctionSet()
//-----------------------------------------------------------------------------
MCAPIFunctionSet::~MCAPIFunctionSet()
{
}

//-----------------------------------------------------------------------------
// Function: getAt()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc const& MCAPIFunctionSet::getAt(unsigned int index) const
{
    Q_ASSERT(index < static_cast<unsigned int>(m_functions.size()));
    return m_functions.at(index);
}

//-----------------------------------------------------------------------------
// Function: find()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc const* MCAPIFunctionSet::find(QString const& name) const
{
    QList<MCAPIFunctionDesc>::const_iterator itrFunc = m_functions.begin();

    while (itrFunc != m_functions.end())
    {
        if (itrFunc->getName() == name)
        {
            return &(*itrFunc);
        }

        ++itrFunc;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: getCount()
//-----------------------------------------------------------------------------
unsigned int MCAPIFunctionSet::getCount() const
{
    return m_functions.size();
}

//-----------------------------------------------------------------------------
// Function: addFunctionDesc()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc* MCAPIFunctionSet::addFunctionDesc(QString const& name, VariableType returnType,
                                                     QString const& returnHint)
{
    Q_ASSERT(find(name) == 0);

    m_functions.push_back(MCAPIFunctionDesc(name, returnType, returnHint));
    return &m_functions.back();
}

//-----------------------------------------------------------------------------
// Function: buildFunctionList()
//-----------------------------------------------------------------------------
void MCAPIFunctionSet::buildFunctionList()
{
    // Build the function list.
    MCAPIFunctionDesc* desc = addFunctionDesc("mcapi_initialize");
    desc->addParam(VAR_TYPE_LOCAL_NODE_ID, "mcapi_node_t node_id");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_version_t* mcapi_version");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_finalize");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_get_node_id", VAR_TYPE_NODE_ID, "mcapi_uint_t");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_create_endpoint", VAR_TYPE_LOCAL_ENDPOINT, "mcapi_endpoint_t");
    desc->addParam(VAR_TYPE_LOCAL_PORT_ID, "mcapi_port_t port_id");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_get_endpoint_i");
    desc->addParam(VAR_TYPE_NODE_ID, "mcapi_node_t node_id");
    desc->addParam(VAR_TYPE_PORT_ID, "mcapi_port_t port_id");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_endpoint_t* endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_get_endpoint", VAR_TYPE_ENDPOINT, "mcapi_endpoint_t");
    desc->addParam(VAR_TYPE_NODE_ID, "mcapi_node_t node_id");
    desc->addParam(VAR_TYPE_PORT_ID, "mcapi_port_t port_id");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_delete_endpoint");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_ANY, "mcapi_endpoint_t endpoint");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_get_endpoint_attribute");
    desc->addParam(VAR_TYPE_ENDPOINT, "mcapi_endpoint_t endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint_t attribute_num");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* attribute");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t attribute_size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_set_endpoint_attribute");
    desc->addParam(VAR_TYPE_ENDPOINT, "mcapi_endpoint_t endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint_t attribute_num");
    desc->addParam(VAR_TYPE_UNDEFINED, "const void* attribute");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t attribute_size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_msg_send_i");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_OUT,
                   "mcapi_endpoint_t send_endpoint");
    desc->addParam(VAR_TYPE_REMOTE_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t buffer_size");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_priority_t priority");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_msg_send");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_OUT,
                   "mcapi_endpoint_t send_endpoint");
    desc->addParam(VAR_TYPE_REMOTE_ENDPOINT | VAR_TYPE_CONN_MSG |VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t buffer_size");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_priority_t priority");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_msg_recv_i");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t buffer_size");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_msg_recv");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t buffer_size");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t* received_size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_msg_available", VAR_TYPE_UNDEFINED, "mcapi_uint_t");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_MSG | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_connect_pktchan_i");
    desc->addParam(VAR_TYPE_ENDPOINT | VAR_TYPE_CONN_PACKET | VAR_TYPE_CONN_OUT,
                   "mcapi_endpoint_t send_endpoint");
    desc->addParam(VAR_TYPE_ENDPOINT | VAR_TYPE_CONN_PACKET | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_open_pktchan_recv_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_recv_hndl_t* recv_handle");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_PACKET | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_open_pktchan_send_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_send_hndl_t* send_handle");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_PACKET | VAR_TYPE_CONN_OUT, "mcapi_endpoint_t  send_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_send_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t size");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_send");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_recv_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "void** buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_recv");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "void** buffer");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t* received_size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_available", VAR_TYPE_UNDEFINED, "mcapi_uint_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_free");
    desc->addParam(VAR_TYPE_UNDEFINED, "void* buffer");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_recv_close_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_pktchan_send_close_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_pktchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_connect_sclchan_i");
    desc->addParam(VAR_TYPE_ENDPOINT | VAR_TYPE_CONN_SCALAR | VAR_TYPE_CONN_OUT,
                   "mcapi_endpoint_t send_endpoint");
    desc->addParam(VAR_TYPE_ENDPOINT | VAR_TYPE_CONN_SCALAR | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_open_sclchan_recv_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t* receive_handle");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_SCALAR | VAR_TYPE_CONN_IN,
                   "mcapi_endpoint_t receive_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_open_sclchan_send_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t* send_handle");
    desc->addParam(VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_CONN_SCALAR | VAR_TYPE_CONN_OUT,
                   "mcapi_endpoint_t send_endpoint");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_send_uint64");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint64_t dataword");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_send_uint32");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint32_t dataword");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_send_uint16");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint16_t dataword");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_send_uint8");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_uint8_t dataword");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_recv_uint64", VAR_TYPE_UNDEFINED, "mcapi_uint64_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_recv_uint32", VAR_TYPE_UNDEFINED, "mcapi_uint32_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_recv_uint16", VAR_TYPE_UNDEFINED, "mcapi_uint16_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_recv_uint8", VAR_TYPE_UNDEFINED, "mcapi_uint8_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_available", VAR_TYPE_UNDEFINED, "mcapi_uint_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_recv_close_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_recv_hndl_t receive_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_sclchan_send_close_i");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_sclchan_send_hndl_t send_handle");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_test", VAR_TYPE_UNDEFINED, "mcapi_boolean_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t* size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");

    desc = addFunctionDesc("mcapi_wait", VAR_TYPE_UNDEFINED, "mcapi_boolean_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t* size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_timeout_t timeout");

    desc = addFunctionDesc("mcapi_wait_any", VAR_TYPE_UNDEFINED, "mcapi_int_t");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t number");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t** requests");
    desc->addParam(VAR_TYPE_UNDEFINED, "size_t* size");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_timeout_t timeout");

    desc = addFunctionDesc("mcapi_cancel");
    desc->addParam(VAR_TYPE_UNDEFINED, "mcapi_request_t* request");
    desc->addParam(VAR_TYPE_STATUS_PTR, "mcapi_status_t* mcapi_status");
}