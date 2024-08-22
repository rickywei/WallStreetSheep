package daemon

import (
	"fmt"
	"net/http"
	"time"

	"github.com/go-resty/resty/v2"
	"github.com/spf13/cast"
	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
)

var (
	Cli = resty.New()

	feishuCh      = make(chan *FeishuWebhook, 1024)
	feishuTkBot   = time.NewTicker(time.Second / 5)
	feishuTkSheet = time.NewTicker(time.Minute / 50)
)

func init() {
	Cli.SetCookieJar(nil)
	Cli.SetRetryCount(3)
}

type FeishuWebhook struct {
	Key  string
	Data any
}

func SendFeishu[T any](key string, data T) {
	feishuCh <- &FeishuWebhook{
		Key:  key,
		Data: data,
	}
}

func RunFeishu() error {
	for {
		select {
		case w := <-feishuCh:
			<-feishuTkBot.C
			<-feishuTkSheet.C
			ret := make(map[string]any)
			resp, err := Cli.R().
				SetBody(w.Data).
				SetResult(&ret).
				SetAuthToken(conf.String(fmt.Sprintf("%s.bearer", w.Key))).
				Post(conf.String(fmt.Sprintf("%s.url", w.Key)))
			if err != nil || resp.StatusCode() != http.StatusOK || cast.ToInt(ret["code"]) != 0 {
				logger.L().Error("send feishu failed", zap.String("resp", resp.String()), zap.Error(err))
			}
		case <-ctx.Done():
			return nil
		}
	}
}

func StopFeishu() {
	defer cancel()
}
