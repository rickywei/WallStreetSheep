package model

import (
	"context"
	"encoding/json"
	"fmt"
	"math"
	"reflect"

	"github.com/redis/go-redis/v9"
	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
	"go.uber.org/zap"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
	"gorm.io/gorm/clause"
)

var (
	RC redis.UniversalClient
	DB *gorm.DB
)

func init() {
	var err error
	RC = redis.NewUniversalClient(&redis.UniversalOptions{
		Addrs: conf.Strings("redis.addr"),
	})
	if _, err = RC.Ping(context.Background()).Result(); err != nil {
		logger.L().Fatal("ping redis failed", zap.Error(err))
	}

	DB, err = gorm.Open(postgres.New(postgres.Config{
		DSN: fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s",
			conf.String("postgres.host"),
			conf.Int("postgres.port"),
			conf.String("postgres.user"),
			conf.String("postgres.password"),
			conf.String("postgres.dbname"),
		),
	}))
	if err != nil {
		logger.L().Fatal("connect postgres failed", zap.Error(err))
	}
}

func WriteDb[T any](ctx context.Context, data T) (err error) {
	rv := reflect.ValueOf(data).Elem()
	n := rv.NumField()
	for i := 0; i < n; i++ {
		v := rv.Field(i)
		if v.Kind() == reflect.Ptr && v.Elem().Kind() == reflect.Float64 && !v.IsZero() && v.Elem().Float() == math.MaxFloat64 {
			v.SetZero()
		}
	}
	if err = DB.Clauses(clause.OnConflict{DoNothing: true}).WithContext(ctx).Create(data).Error; err != nil {
		logger.L().Error("save failed", zap.Any("data", data))
		return
	}

	return
}

func Publish[T any](ctx context.Context, ch string, data *T) (err error) {
	if data == nil {
		return
	}
	bs, err := json.Marshal(data)
	if err != nil {
		logger.L().Error("marshal failed", zap.Any("data", data))
	}
	if err = RC.Publish(ctx, ch, bs).Err(); err != nil {
		logger.L().Error("publish failed", zap.String("channel", ch), zap.Any("data", data))
	}

	return
}
