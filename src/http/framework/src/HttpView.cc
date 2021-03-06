#include "HttpView.h"
#include "TRClassMap.h"


HttpResponse HttpView::genHttpResponse(std::string viewName,const HttpViewData &data)
{
    TRObject *obj=TRClassMap::newObject(viewName);
    if(obj)
    {
        HttpView *view= dynamic_cast<HttpView *>(obj);
        if(view)
        {
            return view->genHttpResponse(data);
            delete view;
        }

    }
    return notFoundResponse();
}
HttpResponse HttpView::notFoundResponse()
{
    HttpResponse resp((true));
    resp.setStatusCode(HttpResponse::k404NotFound);
    resp.setCloseConnection(true);

    return resp;
}
HttpResponse HttpView::locationRedirectResponse(std::string path)
{
    HttpResponse resp((true));
    resp.setStatusCode(HttpResponse::k302Found);
    resp.redirect(path.c_str());
    return resp;
}