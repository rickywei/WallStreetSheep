package daemon

import (
	"context"
	"encoding/json"
	"math"
	"reflect"

	"github.com/redis/go-redis/v9"
	"github.com/rickywei/wallstreetsheep/logger"
	"go.uber.org/zap"
	"gorm.io/gorm"
	"gorm.io/gorm/clause"
)

func writeDb[T any](ctx context.Context, db *gorm.DB, data T) {
	rv := reflect.ValueOf(data).Elem()
	n := rv.NumField()
	for i := 0; i < n; i++ {
		v := rv.Field(i)
		if v.Kind() == reflect.Ptr && v.Elem().Kind() == reflect.Float64 && !v.IsZero() && v.Elem().Float() == math.MaxFloat64 {
			v.SetZero()
		}
	}
	if err := db.Clauses(clause.OnConflict{DoNothing: true}).WithContext(ctx).Create(data).Error; err != nil {
		logger.L().Error("save failed", zap.Any("data", data))
		return
	}
}

func publish[T any](ctx context.Context, rc redis.UniversalClient, ch string, data *T) {
	bs, err := json.Marshal(data)
	if err != nil {
		logger.L().Error("marshal failed", zap.Any("data", data))
	}
	if err = rc.Publish(ctx, ch, bs).Err(); err != nil {
		logger.L().Error("publish failed", zap.String("channel", ch), zap.Any("data", data))
	}
}
