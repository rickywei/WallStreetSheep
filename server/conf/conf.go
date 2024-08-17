package conf

import (
	"github.com/knadh/koanf"
	"github.com/knadh/koanf/parsers/yaml"
	"github.com/knadh/koanf/providers/file"
	"github.com/spf13/pflag"
	"go.uber.org/zap"
)

var (
	k = koanf.New(".")
	p = yaml.Parser()
)

func init() {
	path := pflag.String("conf", "conf.yaml", "")
	pflag.Parse()

	f := file.Provider(*path)
	if err := k.Load(f, p); err != nil {
		zap.L().Fatal("init conf failed", zap.Error(err))
	}
	f.Watch(func(event interface{}, err error) {
		if err != nil {
			zap.L().Fatal("watch conf.yaml failed", zap.Error(err))
			return
		}
		k.Load(f, p)
	})
}

func String(path string) string {
	return k.String(path)
}

func Strings(path string) []string {
	return k.Strings(path)
}

func Int(path string) int {
	return k.Int(path)
}

func Bool(path string) bool {
	return k.Bool(path)
}
